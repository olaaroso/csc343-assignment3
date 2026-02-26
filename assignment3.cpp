#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    const int SIZE = 20;
    int arr[SIZE];
    int fd[2]; // File descriptors for the pipe: fd[0] for reading, fd[1] for writing

    // Seed the random number generator
    srand(time(NULL));

    // Fill the array with random numbers (1 to 100 for readability)
    cout << "Array elements: ";
    for (int i = 0; i < SIZE; ++i) {
        arr[i] = rand() % 100 + 1;
        cout << arr[i] << " ";
    }
    cout << "\n\n";

    // Create the pipe before forking so both processes share the file descriptors
    if (pipe(fd) == -1) {
        cerr << "Pipe creation failed!" << endl;
        return 1;
    }

    // Create the child process
    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed!" << endl;
        return 1;
    }

    if (pid == 0) {
        // -----------------------
        // --- Child Process ---
        // -----------------------
        close(fd[0]); // Close the unused read end of the pipe

        // Find minimum in the second half of the array (indices 10 to 19)
        int child_min = arr[SIZE / 2];
        for (int i = (SIZE / 2) + 1; i < SIZE; ++i) {
            if (arr[i] < child_min) {
                child_min = arr[i];
            }
        }

        cout << "Child Process  (PID: " << getpid() << ") - Minimum in second half: " << child_min << endl;

        // Write the child's minimum to the pipe
        write(fd[1], &child_min, sizeof(child_min));
        close(fd[1]); // Close the write end after sending data
        
        exit(0); // Exit the child process gracefully

    } else {
        // ------------------------
        // --- Parent Process ---
        // ------------------------
        close(fd[1]); // Close the unused write end of the pipe

        // Find minimum in the first half of the array (indices 0 to 9)
        int parent_min = arr[0];
        for (int i = 1; i < SIZE / 2; ++i) {
            if (arr[i] < parent_min) {
                parent_min = arr[i];
            }
        }

        cout << "Parent Process (PID: " << getpid() << ") - Minimum in first half:  " << parent_min << endl;

        // Wait for the child process to finish its execution
        wait(NULL);

        // Read the child's minimum from the pipe
        int child_min_received;
        read(fd[0], &child_min_received, sizeof(child_min_received));
        close(fd[0]); // Close the read end after reading data

        // Compare the two minimums to find the overall minimum
        int overall_min = (parent_min < child_min_received) ? parent_min : child_min_received;

        cout << "\nOverall Minimum of the entire array: " << overall_min << endl;
    }

    return 0;
}