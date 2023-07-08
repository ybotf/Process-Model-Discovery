/* Program to discover a process model from an event log.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  August 2022, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Toby Fung 1356386
  Dated:     06/10/2022

*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define INITIAL_SIZE 2
#define MAX_TRACE_SIZE (26 * 2)
#define IDENTIFIED_PATTERN_CODE 256
#define MAX_INT(a, b) \
    ({int _a = (a), _b = (b); _a > _b ? _a : _b; })
/* Function copied from and written by:
https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html#Statement-Exprs.
*/

/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef unsigned int action_t;                // action identified by an integer
typedef action_t trace_t[MAX_TRACE_SIZE + 1]; // an array of actions

typedef struct // dictionary for the number of each event
{
    int count;
    action_t event;
} char_dict_t;

typedef struct // log for the frequency and size of a trace
{
    int frq;
    int trace_size;
    trace_t one_trace;
} log_t;

typedef struct // struct for storing the most likely choice of action
{              // along with its relevant informations
    int weight;
    int removed_frq;
    char *chosen_action;
    action_t x;
    action_t y;
} seq_t;

/* FUNCTION PROTOTYPES -------------------------------------------------------*/
int check_distinct_char(char_dict_t *event_dict, int target, int nd_event);
char_dict_t *insert_dict(char_dict_t *event_dict, int target,
                         int *nd_event, size_t *dict_size);
void remove_dict(char_dict_t *event_dict, int *nd_event, action_t z);
void update_dict_freq(log_t *trace_log, int nd_trace,
                      char_dict_t *event_dict, int nd_event);
int update_event_count(char_dict_t *event_dict, int nd_event);
int check_distinct_trace(log_t *trace_log, trace_t one_trace,
                         int nd_trace, int trace_size);
log_t *insert_trace(log_t *trace_log, trace_t one_trace, int *nd_trace,
                    size_t *log_size, int trace_size);
void remove_trace(seq_t seq, log_t *trace_log, int nd_trace, int pattern);
int sup_search(log_t *trace_log, int nd_trace, action_t x, action_t y);
void fill_matrix(int (*f)(log_t *, int, action_t, action_t),
                 action_t *matrix, log_t *trace_log, int nd_trace,
                 char_dict_t *event_dict, int nd_event);
seq_t choice_of_action(action_t *matrix, int nd_event,
                       char_dict_t *event_dict, int tot_event, int stage);
int cmp_char(const void *a, const void *b);
int cmp_str_lexi(const void *a, const void *b);
int cmp_int(const void *a, const void *b);
void print_stage_0(void (*f)(int, char_dict_t *),
                   int nd_event, int nd_trace, int tot_event, int tot_trace,
                   char_dict_t *event_dict, log_t *trace_log);
void print_dict(int nd_event, char_dict_t *event_dict);
void print_matrix(action_t *matrix, int nd_event);
void print_choice(seq_t seq, int pattern);

/* MAIN FUNCTION -------------------------------------------------------------*/
int main(int argc, char *argv[])
{ // number of distinct event and trace, total number of event and trace
    int nd_event = 0, nd_trace = 0, tot_event = 0, tot_trace = 0;
    size_t dict_size = INITIAL_SIZE, log_size = INITIAL_SIZE, trace_size = 0;
    trace_t one_trace;       // array of events
    log_t *trace_log;        // array of logs
    char_dict_t *event_dict; // array of dicts
    trace_log = (log_t *)malloc(log_size * sizeof(*trace_log));
    event_dict = (char_dict_t *)malloc(dict_size * sizeof(*event_dict));
    action_t c = 0;
    while (c != EOF) // looping through text file
    {
        c = getchar();
        if (isalpha(c)) // if action_t is a char
        {
            if (!check_distinct_char(event_dict, c, nd_event)) // if not found
            {                                                  // in char_dict
                event_dict = insert_dict(event_dict, c, &nd_event, &dict_size);
                event_dict[nd_event - 1].count = 1;
            }
            one_trace[trace_size] = c;
            tot_event += 1, trace_size += 1;
        }
        else if (c == '\n' || c == EOF)
        {
            if (!check_distinct_trace(trace_log, one_trace,
                                      nd_trace, trace_size))
            { // if trace not in trace_log
                trace_log = insert_trace(trace_log, one_trace,
                                         &nd_trace, &log_size, trace_size);
            }
            memset(one_trace, 0, trace_size);
            trace_size = 0, tot_trace += 1;
        }
    }
    qsort(trace_log, nd_trace, sizeof(log_t), cmp_str_lexi);    // sorting trace
    qsort(trace_log, nd_trace, sizeof(log_t), cmp_int);         // lexically
    qsort(event_dict, nd_event, sizeof(char_dict_t), cmp_char); // sorting dict
    print_stage_0(print_dict, nd_event, nd_trace,
                  tot_event, tot_trace, event_dict, trace_log);
    int stage = 0;
    printf("==STAGE 1============================\n");
    action_t matrix[nd_event + 1][nd_event + 1]; // creating 2D matrix
    seq_t seq;
    action_t pattern = IDENTIFIED_PATTERN_CODE;
    fill_matrix(sup_search, *matrix, trace_log, nd_trace, event_dict, nd_event);
    seq = choice_of_action(*matrix, nd_event, event_dict, tot_event, stage);
    while (!strcmp(seq.chosen_action, "SEQ") && // looping through array and
           isalpha(seq.x) && isalpha(seq.y))    // deciding the choice of action
    {
        print_matrix(*matrix, nd_event), print_choice(seq, pattern);
        printf("Number of events removed: %d\n", seq.removed_frq);
        remove_trace(seq, trace_log, nd_trace, pattern);
        event_dict = insert_dict(event_dict, pattern, &nd_event, &dict_size);
        update_dict_freq(trace_log, nd_trace, event_dict, nd_event);
        remove_dict(event_dict, &nd_event, seq.x);
        remove_dict(event_dict, &nd_event, seq.y);
        qsort(event_dict, nd_event, sizeof(char_dict_t), cmp_char);
        print_dict(nd_event, event_dict);

        tot_event = update_event_count(event_dict, nd_event);
        pattern += 1;
        fill_matrix(sup_search, *matrix, trace_log,
                    nd_trace, event_dict, nd_event);
        seq = choice_of_action(*matrix, nd_event, event_dict, tot_event, stage);

        if (strcmp(seq.chosen_action, "SEQ") ||
            !isalpha(seq.x) || !isalpha(seq.y))
            break;
        printf("=====================================\n");
    }
    printf("==STAGE 2============================\n");
    stage = 1;
    while (nd_event > 1)
    {
        print_matrix(*matrix, nd_event), print_choice(seq, pattern);
        printf("Number of events removed: %d\n", seq.removed_frq);
        remove_trace(seq, trace_log, nd_trace, pattern);
        event_dict = insert_dict(event_dict, pattern, &nd_event, &dict_size);
        update_dict_freq(trace_log, nd_trace, event_dict, nd_event);
        remove_dict(event_dict, &nd_event, seq.x);
        remove_dict(event_dict, &nd_event, seq.y);
        qsort(event_dict, nd_event, sizeof(char_dict_t), cmp_char);
        print_dict(nd_event, event_dict);

        tot_event = update_event_count(event_dict, nd_event);
        pattern += 1;
        fill_matrix(sup_search, *matrix, trace_log,
                    nd_trace, event_dict, nd_event);
        seq = choice_of_action(*matrix, nd_event, event_dict, tot_event, stage);

        if (!(nd_event > 1))
            break;
        printf("=====================================\n");
    }
    printf("==THE END============================\n");

    free(event_dict), free(trace_log);
    return EXIT_SUCCESS; // remember, algorithms are fun!!!
}

/* FUNCTIONS -----------------------------------------------------------------*/
int check_distinct_char(char_dict_t *event_dict, int target, int nd_event)
{
    if (!nd_event) // return 0 if there are no events
        return 0;

    for (size_t i = 0; i < nd_event; i++)
    {
        if (event_dict[i].event == target) // if matched
        {
            event_dict[i].count += 1;
            return 1;
        }
    }
    return 0;
}
char_dict_t *insert_dict(char_dict_t *event_dict, int target,
                         int *nd_event, size_t *dict_size)
{
    if (*nd_event == *dict_size) // check if there is enough memory
    {
        *dict_size *= 2;
        event_dict = realloc(event_dict, *dict_size * sizeof(*event_dict));
    }
    event_dict[*nd_event].event = target; // inserting element into dict
    *nd_event += 1;

    return event_dict;
}
void update_dict_freq(log_t *trace_log, int nd_trace,
                      char_dict_t *event_dict, int nd_event)
{
    action_t next_event, iter;
    for (size_t i = 0; i < nd_event; i++)
    {
        event_dict[i].count = 0; // resetting dictionary counts to 0
        for (size_t j = 0; j < nd_trace; j++)
        {

            for (size_t k = 0; k < trace_log[j].trace_size; k++)
            {
                iter = 1;
                next_event = trace_log[j].one_trace[k + 1];
                // finding consecutive events
                while (next_event == 0 && k + iter < trace_log[j].trace_size)
                {
                    next_event = trace_log[j].one_trace[k + 1 + iter];
                    iter += 1;
                }
                // checking if events matches
                if ((trace_log[j].one_trace[k] == event_dict[i].event) &&
                    (trace_log[j].one_trace[k] != next_event))
                {
                    event_dict[i].count += trace_log[j].frq;
                }
            }
        }
    }
}
void remove_dict(char_dict_t *event_dict, int *nd_event, action_t z)
{
    int remove_pos = -1;
    for (size_t i = 0; i < *nd_event; i++) // finding the removal position
    {
        if (event_dict[i].event == z)
        {
            remove_pos = i;
            break;
        }
    }
    for (size_t i = remove_pos; i < *nd_event - 1; i++) // filling in the
    {                                                   // removed position
        event_dict[i] = event_dict[i + 1];
    }
    *nd_event -= 1;
}
int update_event_count(char_dict_t *event_dict, int nd_event)
{
    int sum = 0;
    for (size_t i = 0; i < nd_event; i++) // looping and re-counting the
    {                                     // total number of events
        if (event_dict[i].event)
            sum += event_dict[i].count;
    }
    return sum;
}

int check_distinct_trace(log_t *trace_log, trace_t one_trace,
                         int nd_trace, int trace_size)
{
    if (!nd_trace) // return 0 if there are no traces
        return 0;

    for (size_t i = 0; i < nd_trace; i++)
    {
        int flag = 1;
        if (trace_log[i].trace_size == trace_size)
        {
            for (size_t j = 0; j < trace_size; j++)
            {
                if (trace_log[i].one_trace[j] != one_trace[j])
                    flag = 0; // if event already exists
            }
            if (flag) // if event does not exist
            {
                trace_log[i].frq += 1;
                return 1;
            }
        }
    }
    return 0;
}
log_t *insert_trace(log_t *trace_log, trace_t one_trace, int *nd_trace,
                    size_t *log_size, int trace_size)
{
    if (*nd_trace == *log_size) // check if there is enough memory
    {
        *log_size *= 2;
        trace_log = realloc(trace_log, *log_size * sizeof(*trace_log));
    }
    for (size_t i = 0; i < trace_size; i++)
    {
        trace_log[*nd_trace].one_trace[i] = one_trace[i]; // inserting element
    }
    trace_log[*nd_trace].frq = 1;
    trace_log[*nd_trace].trace_size = trace_size;
    *nd_trace += 1;

    return trace_log;
}
void remove_trace(seq_t seq, log_t *trace_log, int nd_trace, int pattern)
{
    int iter = 0;
    for (size_t i = 0; i < nd_trace; i++)
    {
        for (size_t j = 0; j < (trace_log[i].trace_size); j++)
        {
            if (trace_log[i].one_trace[j] == seq.x || // assigning all matched
                trace_log[i].one_trace[j] == seq.y)   // events to pattern no.
            {
                trace_log[i].one_trace[j] = pattern;
            }
        }
    }
    for (size_t i = 0; i < nd_trace; i++)
    {
        for (size_t j = 0; j < (trace_log[i].trace_size); j++)
        {
            iter = 1;
            while ((trace_log[i].one_trace[j] == pattern) && // pattern
                   (trace_log[i].one_trace[j] ==             // abstraction
                        trace_log[i].one_trace[j + iter] ||
                    trace_log[i].one_trace[j + iter] == 0))
            {
                trace_log[i].one_trace[j + iter] = 0;
                iter += 1;
            }
        }
    }
}
int cmp_char(const void *a, const void *b) // comparing char value
{
    char_dict_t *char_dictA = (char_dict_t *)a;
    char_dict_t *char_dictB = (char_dict_t *)b;
    return (char_dictA->event - char_dictB->event);
}
int cmp_int(const void *a, const void *b) // comparing int value
{
    log_t *trace_logA = (log_t *)a;
    log_t *trace_logB = (log_t *)b;
    return ((trace_logB->frq - trace_logA->frq));
}
int cmp_str_lexi(const void *a, const void *b) // comparing string value
{                                              // in lexical order
    char wordA[MAX_TRACE_SIZE + 1], wordB[MAX_TRACE_SIZE + 1];
    log_t *trace_logA = (log_t *)a;
    log_t *trace_logB = (log_t *)b;
    for (size_t i = 0; i < MAX_TRACE_SIZE; i++)
    {
        wordA[i] = trace_logA->one_trace[i];
        wordB[i] = trace_logB->one_trace[i];
    }
    return (strcmp(wordA, wordB));
}

int sup_search(log_t *trace_log, int nd_trace, action_t x, action_t y)
{
    if (x == y) // sequential event can't be equal to each other
        return 0;

    action_t a, b;
    int tot_sup = 0;
    for (size_t i = 0; i < nd_trace; i++)
    {
        for (size_t j = 0; j < (trace_log[i].trace_size) - 1; j++)
        {
            int iter = 1;
            a = trace_log[i].one_trace[j];     // first event,
            b = trace_log[i].one_trace[j + 1]; // following event
            while (a && b == 0 && (j + 1 + iter) < trace_log[i].trace_size)
            {
                b = trace_log[i].one_trace[j + 1 + iter]; // looping until the
                iter += 1;                                // follow event is
            }                                             // non-zero
            if (x == a && y == b)
            {
                tot_sup += trace_log[i].frq; // if seq order is found
            }
        }
    }
    return tot_sup;
}
void fill_matrix(int (*f)(log_t *, int, action_t, action_t),
                 action_t *matrix, log_t *trace_log, int nd_trace,
                 char_dict_t *event_dict, int nd_event)
{
    action_t x, y;
    for (size_t row = 0; row < nd_event + 1; row++)
    {
        for (size_t col = 0; col < nd_event + 1; col++)
        {
            if ((!col && !row)) // settting [0][0] element to 0
            {
                *((matrix + row * (nd_event + 1)) + col) = 0;
            }
            else if (col == 0) // setting top row of events
            {
                if ((event_dict[row - 1].event) > 0)
                    *((matrix + row * (nd_event + 1)) + col) =
                        event_dict[row - 1].event;
            }
            else if (row == 0) // setting top colum of events
            {
                if ((event_dict[col - 1].event) > 0)
                    *((matrix + row * (nd_event + 1)) + col) =
                        event_dict[col - 1].event;
            }
            else
            {
                x = event_dict[row - 1].event;
                y = event_dict[col - 1].event;
                if (x != 0 && y != 0) // if the seq order is non-zero
                    *((matrix + row * (nd_event + 1)) + col) =
                        (*f)(trace_log, nd_trace, x, y); // (*f) -> sup_search
            }
        }
    }
}
seq_t choice_of_action(action_t *matrix, int nd_event,
                       char_dict_t *event_dict, int tot_event, int stage)
{
    seq_t seq;
    seq.weight = 0;
    int pd, w, sup, rev_sup;
    char *action;
    for (size_t row = 0; row < nd_event + 1; row++)
    {
        for (size_t col = 0; col < nd_event + 1; col++)
        {
            if (!(!row && !col) && !(!row || !col) && // testing for non-zeros
                event_dict[row - 1].event != event_dict[col - 1].event)
            {
                sup = *((matrix + row * (nd_event + 1)) + col);     // >sup(x,y)
                rev_sup = *((matrix + col * (nd_event + 1)) + row); // >sup(y,x)

                if (sup + rev_sup == 0 || sup != 0 || stage)
                {
                    if (MAX_INT(sup, rev_sup))
                    {
                        pd = 100 * abs(sup - rev_sup) / MAX_INT(sup, rev_sup);
                    }
                    else // if sup(x,y) and sup(x,y) is 0, skip divide by 0
                    {
                        pd = 0;
                    }
                    if (MAX_INT(sup, rev_sup) <= (tot_event / 100))
                    {
                        w = tot_event * 100;
                        action = "CHC";
                    }
                    else if (pd > 70)
                    {
                        w = abs(50 - pd) * MAX_INT(sup, rev_sup);
                        if (isalnum(event_dict[row - 1].event) &&
                            isalnum(event_dict[col - 1].event))
                        {
                            w *= 100;
                        }
                        action = ("SEQ");
                    }
                    else if (pd < 30 || stage)
                    {
                        w = 100 * abs(50 - pd) * MAX_INT(sup, rev_sup);
                        action = "CON";
                    }
                    if (w > seq.weight)
                    {
                        seq.weight = w;
                        seq.chosen_action = action;
                        seq.removed_frq = sup + rev_sup;
                        seq.x = event_dict[row - 1].event;
                        seq.y = event_dict[col - 1].event;
                    }
                }
            }
        }
    }
    return seq;
}

void print_dict(int nd_event, char_dict_t *event_dict)
{
    for (size_t i = 0; i < nd_event; i++)
    {
        if (isalpha(event_dict[i].event) && event_dict[i].count)
            printf("%c = %d\n", event_dict[i].event, event_dict[i].count);

        else if (event_dict[i].count)
            printf("%d = %d\n", event_dict[i].event, event_dict[i].count);
    }
}
void print_stage_0(void (*f)(int, char_dict_t *),
                   int nd_event, int nd_trace, int tot_event, int tot_trace,
                   char_dict_t *event_dict, log_t *trace_log)
{
    int flag = 1, i = 0;
    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", nd_event);
    printf("Number of distinct traces: %d\n", nd_trace);
    printf("Total number of events: %d\n", tot_event);
    printf("Total number of traces: %d\n", tot_trace);
    printf("Most frequent trace frequency: %d\n", trace_log[0].frq);
    while (flag && i < nd_trace)
    {
        for (size_t j = 0; j < trace_log[i].trace_size; j++)
        {
            printf("%c", trace_log[i].one_trace[j]);
        }

        if (i + 1 == nd_trace || trace_log[i].frq != trace_log[i + 1].frq)
            flag = 0;

        i += 1;
        printf("\n");
    }
    print_dict(nd_event, event_dict);
}
void print_matrix(action_t *matrix, int nd_event)
{
    action_t temp;
    for (size_t row = 0; row < nd_event + 1; row++)
    {
        for (size_t col = 0; col < nd_event + 1; col++)
        {
            if (!row && !col)
            {
                printf("%5c", ' ');
            }
            else if (!row || !col)
            {
                temp = *((matrix + row * (nd_event + 1)) + col);
                if (isalpha(temp))
                {

                    printf("%5c", temp);
                }
                else
                {
                    printf("%5d", temp);
                }
            }
            else
            {
                printf("%5d", *((matrix + row * (nd_event + 1)) + col));
            }
        }
        printf("\n");
    }
    printf("-------------------------------------\n");
}
void print_choice(seq_t seq, int pattern)
{
    if (isalpha(seq.x) && isalpha(seq.y))
        printf("%d = %s(%c,%c)\n", pattern, seq.chosen_action, seq.x, seq.y);

    else if (!isalpha(seq.x) && isalpha(seq.y))
        printf("%d = %s(%d,%c)\n", pattern, seq.chosen_action, seq.x, seq.y);

    else if (isalpha(seq.x) && !isalpha(seq.y))
        printf("%d = %s(%c,%d)\n", pattern, seq.chosen_action, seq.x, seq.y);

    else if (!isalpha(seq.x) && !isalpha(seq.y))
        printf("%d = %s(%d,%d)\n", pattern, seq.chosen_action, seq.x, seq.y);
}
