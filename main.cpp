#include <iostream>
#include <vector>
#include <thread>
#include <future>

#define SAVE_PRIMES false

using namespace std;

vector<int> primes;
int primes_count = 0;

int get_primes(int start, int end) {
    thread::id id = this_thread::get_id();
    int half_of_current_number;
    int divisible_count;
    vector<int> prime_numbers;
    int prime_count = 0;

    for (int current_number = start; current_number < end; current_number++) {
        divisible_count = 0;
        half_of_current_number = current_number / 2;
        for (int i = 1; i <= current_number; ++i) {
            // If the current number is divisible by i, it is not prime
            if (current_number % i == 0) {
                divisible_count++;
            }
            // To find a prime number we only need to reach the middle of the number
            if (half_of_current_number == i) {
                divisible_count++;
                break;
            }
            // If the number reached more than 2 divisors, it is not prime
            if (divisible_count > 2) {
                break;
            }
        }
        // If the number has 2 divisors, it is prime
        if (divisible_count == 2) {
            if (SAVE_PRIMES) {
                primes.push_back(current_number);
            }
            prime_count++;
        }
    }
    if (SAVE_PRIMES) {
        primes.insert(primes.end(), prime_numbers.begin(), prime_numbers.end());
    } else {
        primes_count += prime_count;
    }
    cout << "Thread " << id << " - " << start << " - " << end << " finished and found " << prime_count << " prime numbers" << endl;
    return 0;
}

int main() {
    cout << "Starting discovering primes ..." << endl;

    int number_of_threads = 10;
    int batch_size = 1000;
    int check_unit = 10000;
    // Create vector for threads and reserve number_of_threads of spaces
    vector<future<int>> threads;
    threads.reserve(number_of_threads);

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

    return 0;
}