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
    int fd[2]; 

    srand(time(NULL));

    cout << "Array elements: ";
    for (int i = 0; i < SIZE; ++i) {
        arr[i] = rand() % 100 + 1;
        cout << arr[i] << " ";
    }
    cout << "\n\n";

    if (pipe(fd) == -1) {
        cerr << "Pipe creation failed!" << endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed!" << endl;
        return 1;
    }

    if (pid == 0) {
        close(fd[0]);

        int child_min = arr[SIZE / 2];
        for (int i = (SIZE / 2) + 1; i < SIZE; ++i) {
            if (arr[i] < child_min) {
                child_min = arr[i];
            }
        }

        cout << "Child Process  (PID: " << getpid() << ") - Minimum in second half: " << child_min << endl;

        write(fd[1], &child_min, sizeof(child_min));
        close(fd[1]);
        
        exit(0); 

    } else {
        close(fd[1]); 

        int parent_min = arr[0];
        for (int i = 1; i < SIZE / 2; ++i) {
            if (arr[i] < parent_min) {
                parent_min = arr[i];
            }
        }

        cout << "Parent Process (PID: " << getpid() << ") - Minimum in first half:  " << parent_min << endl;

        wait(NULL);

        int child_min_received;
        read(fd[0], &child_min_received, sizeof(child_min_received));
        close(fd[0]); // Close the read end after reading data

        int overall_min = (parent_min < child_min_received) ? parent_min : child_min_received;

        cout << "\nOverall Minimum of the entire array: " << overall_min << endl;
    }

    return 0;
}