#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <fstream>
#include <sys/stat.h>

using namespace std;

vector<int> primes;
int primes_count = 0;
bool SAVE_PRIMES = false;

int get_primes(int start, int end) {
    thread::id id = this_thread::get_id();
    int half_of_current_number;
    int divisible_count;
    vector<int> prime_numbers;
    int prime_count = 0;

    for (int current_number = start; current_number < end; current_number++) {
        divisible_count = 0;

        // We can exclude numbers that are divisible by 5 or 2
        if ((current_number != 2 && current_number != 5) && (current_number % 5 == 0 || current_number % 2 == 0)) {
            continue;
        }

        half_of_current_number = current_number / 2;
        for (int i = 1; i <= half_of_current_number; i++) {
            // If the current number is divisible by i, it is not prime
            if (current_number % i == 0) {
                divisible_count++;
            }
            // If the number reached more than 2 divisors, it is not prime
            if (divisible_count > 2) {
                break;
            }
        }
        // We always add 1 more divisor because it will always be divisible by it self
        divisible_count++;

        // If the number has 2 divisors, it is prime
        if (divisible_count == 2) {
            if (SAVE_PRIMES) {
                prime_numbers.push_back(current_number);
            }
            prime_count++;
        }
    }
    primes.insert(primes.end(), prime_numbers.begin(), prime_numbers.end());
    primes_count += prime_count;
    cout << "Thread " << id << " - " << start << " - " << end << " finished and found " << prime_count << " prime numbers" << endl;

    return 0;
}

bool folderExists(const string& path) {
    struct stat info{};
    // Check if folder exists
    if (info.st_mode & S_IFDIR) {
        return true;
    }
    return false;
}

void createFolder(const string& path) {
    // Check if folder already exists
    if (!folderExists(path)) {
        // Create folder
        mkdir(path.c_str(), 0777);
    }
}

int main() {
    cout << "Starting discovering primes ..." << endl;

    int number_of_threads = 10;
    int batch_size = 10000;
    int check_unit = 6000000;
    // Create vector for threads and reserve number_of_threads of spaces
    vector<future<int>> threads;
    threads.reserve(number_of_threads);

    // Create output folder
    createFolder("./output");

    // Ask user if they want to save primes to file
    cout << "Should we save primes? (y/n)" << endl;
    char save_primes;
    cin >> save_primes;
    if (save_primes == 'y') {
        SAVE_PRIMES = true;
        cout << "Primes will be saved to ./output/primes.txt" << endl;
    }

    cout << "------------------------------------" << endl;
    cout << "Limit of threads: " << number_of_threads << endl;
    cout << "Total threads: " << check_unit / batch_size << endl;
    cout << "Batch size: " << batch_size << endl;
    cout << "Check unit: " << check_unit << endl;
    cout << "------------------------------------" << endl;

    // Create threads
    for (int x = 0; x < check_unit; x += batch_size) {
        if(threads.size() > number_of_threads) {
            /// If the number of running threads is bigger then the max of threads,
            /// wait for any of the threads to finish and remove it from the vector
            while (threads.size() >= number_of_threads) {
                for (int i = 0; i < threads.size(); i++) {
                    // If the thread is finished, remove it from the vector
                    if (threads[i].wait_for(chrono::seconds(0)) == future_status::ready) {
                        threads.erase(threads.begin() + i);
                    }
                }
            }
        }
        // Create a thread and store it in the vector
        threads.push_back(async(launch::async, get_primes, x, x + batch_size));
    }

    // Wait for all threads to finish
    while (!threads.empty()) {
        for (int i = 0; i < threads.size(); i++) {
            // If the thread is finished, remove it from the vector
            if (threads[i].wait_for(chrono::seconds(0)) == future_status::ready) {
                threads.erase(threads.begin() + i);
            }
        }
    }

    cout << "Prime numbers found: " << primes_count << endl;

    // Create/Open output file inside output folder
    ofstream output_file;
    output_file.open("./output/primes.txt");
    cout << "Writing primes to file ..." << endl;
    for (int prime : primes) {
        output_file << prime << endl;
    }
    cout << "Finished writing primes to file" << endl;
    output_file.close();

    return 0;
}
