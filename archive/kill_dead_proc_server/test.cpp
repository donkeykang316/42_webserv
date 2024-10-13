#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <string>
#include <sstream>
#include <fcntl.h>

void executeCGI(const std::string& scriptPath, const std::string& method,
                const std::string& queryString, const std::string& body,
                std::map<std::string, std::string>& envVars, std::string& cgiOutput) {
    int inPipe[2];
    int outPipe[2];

    if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Child process
        // Redirect stdin and stdout
        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);

        close(inPipe[1]);
        close(outPipe[0]);

        // Set environment variables
        for (std::map<std::string, std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it) {
            setenv(it->first.c_str(), it->second.c_str(), 1);
        }

        // Execute the CGI script
        execl(scriptPath.c_str(), scriptPath.c_str(), NULL);

        // If execl fails
        perror("execl");
        exit(1);
    } else {
        // Parent process
        close(inPipe[0]);
        close(outPipe[1]);

        // Write to CGI stdin if needed
        if (method == "POST") {
            write(inPipe[1], body.c_str(), body.size());
        }
        close(inPipe[1]);

        // Read CGI output
        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
            cgiOutput.append(buffer, bytesRead);
        }
        close(outPipe[0]);

        // Wait for child process to finish
        int status;
        waitpid(pid, &status, 0);
    }
}


