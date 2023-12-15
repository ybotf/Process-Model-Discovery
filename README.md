# Process Model Discovery Program

This program is designed to discover a process model from an event log. The initial skeleton was provided by Artem Polyvyanyy, with the intention that students would modify it to add functionality as required by the assignment specification. For a detailed explanation see: [ass2.pdf](https://github.com/ybotf/Process-Model-Discovery/files/12012325/ass2.pdf)

## Overview

The program discovers a process model from an event log using a sequence of stages. Each stage refines the process model based on identified patterns in the log.

## Compilation and Execution

To compile and execute the program, use the following commands:

```bash
gcc process_model_discovery.c -o process_model_discovery
./process_model_discovery < input_event_log.txt
```

Replace `input_event_log.txt` with the path to your event log file.

## Program Structure

The program is structured as follows:

- **Main Function:** The main function orchestrates the execution of the program, handling file input, data structures, and stages of process model discovery.

- **Functions:** Several functions are defined to perform specific tasks, such as updating dictionaries, inserting and removing traces, filling matrices, and making choices of actions. These functions contribute to the overall process of discovering the process model.

- **Data Structures:** The program uses various data structures, including dictionaries, logs, and matrices, to store and manipulate information about events and traces.

## Stages of Process Model Discovery

1. **Stage 0: Initialization and Data Overview**
   - Displays basic information about the input data, such as the number of distinct events and traces.

2. **Stage 1: Sequence Discovery**
   - Identifies sequential patterns in the event log and updates the process model accordingly.

3. **Stage 2: Concurrency Discovery**
   - Discovers concurrent patterns in the event log and refines the process model.

4. **Conclusion**
   - Displays the final process model and concludes the program.

## Output

The program generates output for each stage, presenting relevant information, matrices, and the chosen actions. The output provides insights into the discovered process model and the patterns identified in the input event log.

## Acknowledgments

This program is based on the initial skeleton provided by Artem Polyvyanyy, and modifications have been made by the student author as required by the assignment specifications.
