//#include "prps.h"
// 65 - 90: A - Z
// 97 - 122: a - z
typedef struct FindList
{
    char *prps, *arg;
    struct FindList *next;
    int is, is_not;
} FindList;

typedef struct ClauseList
{   //PrpsTree **clause;
    char ***clause_array;
    int array_len;
    struct ClauseList *next;
} ClauseList;

void GetLeftMostByte(int *f, int *nmbr)
{
    *nmbr = *f;
    *f = *f >> 8;
    *f = *f << 8;

    *nmbr = *nmbr & ~(*f);
}

void GenSequentialSymbol(char **symbol, int f, int m)
{
    /* This function wou
        m is the number of chars needed in the symbol
     */
    int i, j, value;
    unsigned long int max_val;
    if (symbol == NULL)
        NoMallocErr("GenSequentialSymbols 1");

    if ((*symbol) == NULL)
    {   (*symbol) = calloc(m, sizeof(char));
        if ((*symbol) == NULL)
            MallocErr("GenSequentialSymbol 2");
    }

    max_val = ((unsigned long int) powl(2, 8*m + 1)) - 1;

    if (f > max_val)
        InconsistencyErr("GenSequentialSymbol 3");

    i = 0;
    while (f > 0)
    {
        GetLeftMostByte(&f, &value);
        value += 65;
        (*symbol)[i++] = value;
    }

    (*symbol)[i] = '\0';
}

void GenSymbol(char **symbol, int n)
{
    int i, value;
    if (symbol == NULL)
        NoMallocErr("GenSymbols 1");

    if ((*symbol) == NULL)
    {
        (*symbol) = calloc(n, sizeof(char));
        if ((*symbol) == NULL)
            MallocErr("GenSymbol 2");
    }

    for (i = 0; i < n - 1; i++)
    {   value = rand() % 26;
        value += 65;
        (*symbol)[i] = value;
    }

    (*symbol)[n-1] = '\0';
}

void GenSymbolArray(char ***symbols, unsigned long int n, unsigned int m)
{   
    /*  Generate an array of n strings
     *
     *  Not sure if the m argument (arg 2) is useful. Supposedly it is 
     *  the max # of chars per string, but if its too small it gets set 
     *  to the min amount
     */

    unsigned long int i, j;
    //unsigned long int m;
    char *symbolStore = NULL;
    char *buffer = NULL;
    unsigned long int min_m = (unsigned int) ceil(log2l(n) / 3);

    if (m < min_m)
        m = min_m;

    if (symbols == NULL)
        NoMallocErr("GenSymbolArray");

    symbolStore = calloc((m + 1) * n, sizeof(char));
    (*symbols) = calloc(n, sizeof(char *));
    //buffer = calloc(n, sizeof(char));

    if ((*symbols) == NULL)
        MallocErr("GenSymbolArray 2");
    if (symbolStore == NULL)
        MallocErr("GenSymbolArray 3");
    //if (buffer == NULL)
    //    MallocErr("GenSymbolArray 4");

    for (i = 0; i < n; i++)
    {   (*symbols)[i] = &(symbolStore[i * (m + 1)]);
        GenSequentialSymbol(&((*symbols)[i]), i+1, m);
    }
}

//void GenArgArray(char ***args, unsigned long int n, unsigned long int M)
//{
//
//}

void FreeSymbolArray(char ***symbols, unsigned long int n, unsigned int m)
{
    unsigned long int i, j;

    if (symbols != NULL)
    {
        free(**symbols);
        free(*symbols);

        **symbols = NULL;
        *symbols = NULL;
    }
}

void GenSkewedTree(const char **symbols, const char **args, int m, int n, PrpsTree **tree, int d)
{

    int i, j, index;
    const char *ptr = NULL, *arg = NULL;
    long double nmbr;
    PrpsTree **left = NULL, **right = NULL;
    //if (tree == NULL)

    if ((*tree) == NULL)
    {   (*tree) = GenerateEmpty();
        index = rand() % 4;
        ptr = symbols[index];

        if (d > 0)
        {
            index = rand() % 4;
        	AllocateAsOprtr(tree, index);

            left = calloc(1, sizeof(PrpsTree *));
            right = calloc(1, sizeof(PrpsTree *));

            if (!left || !right)
                MallocErr("GenSkewedTree 1");

            nmbr = (long double) rand();
            nmbr = (d - 1) * ((int) rintl(logl(nmbr) / logl((long double) RAND_MAX)));

            GenSkewedTree(symbols, args, m, n, left, d - 1);
            GenSkewedTree(symbols, args, m, n, right, d - 1 - nmbr);

            SetLeftChild(tree, left);
            SetRightChild(tree, right);

            free(left);
            free(right);

            left = NULL;
            right = NULL;
        }

        else
        {   
            j = rand() % m;
            ptr = symbols[j];
            j = rand() % n;
            arg = args[j];

            AllocateAsPrps(tree, ptr, arg);

            //for (i = 0; i < d; i++)
            //{   
            //    tree = Oprtr2
            //           (rand() % 3, tree,
            //            rand() % 3, ptr,
            //            rand() % 3, rand() % 4);
            //}
        }

        if (rand() > RAND_MAX / 2)
            Negate(tree);
    }
}

void GenAlphaTree(const char **symbols, const char **args, PrpsTree ***alpha, int m, int n, int d)
{
    int i, j;
    const char *symbol_tmp = NULL, *arg_tmp;
    //PrpsTree **alpha = NULL;

    if (alpha == NULL)
        NoMallocErr("GenAlphaTree 1");

    if ((*alpha) != NULL)
        InconsistencyErr("GenAlphaTree 2");

    (*alpha) = calloc(1, sizeof(PrpsTree *));
    if ((*alpha) == NULL)
        MallocErr("GenAlphaTree 3");
    (**alpha) = GenerateEmpty();

    i = rand() % m;
    symbol_tmp = symbols[i];

    i = rand() % n;
    arg_tmp = args[i];

    AllocateAsPrps(*alpha, symbol_tmp, arg_tmp);
}

void FillHashTable(HashTable **hash, char **symbols, char **args, int m, int n, int seed)
{
    int i, j, value, value_retrieved;
    int a, b;
    int iterations;

    srand(seed);

    for (i = 0; i < m*50; i++)
    {
        iterations = 50 * n; //rand() % n;

        for (j = 0; j < iterations; j++)
        {
            value = rand() % 2; 
            a = rand() % m;
            b = rand() % n;

            SetValueForArg(symbols[a], args[b], value, hash, m, n);

            value_retrieved = GetValueForArg(symbols[a], args[b], hash, m, n);

            if (value_retrieved != value)
                InconsistencyErr("FillHashTable 1: Unit Test! ");
        }
    }
}

void AddStmnt(FindList **list, char *prps, char *arg, int neg)
{
    /*  
     */
    FindList **head = NULL;
    int return_val = 0;

    head = list;
    while (head != NULL && (*head) != NULL)
    {
        if (strcmp((*head) -> prps, prps) == 0 && strcmp((*head) -> arg, arg) == 0)
        {
            if (neg == 0)
                (*list) -> is += 1;
            else if (neg == 1)
                (*list) -> is_not += 1;
            break;
        }

        head = &((*head) -> next);
    }

    if ((*head) == NULL)
    {
        (*head) = calloc(1, sizeof(FindList));
        if ((*head) == NULL)
            MallocErr("AddStmnt 1");

        (*head) -> is = 0;
        (*head) -> is_not = 0;

        if (neg == 0)
            (*head) -> is = 1;
        else if (neg == 1)
            (*head) -> is_not = 1;

        (*head) -> prps = calloc(2, sizeof(char));
        (*head) -> arg = calloc(2, sizeof(char));
        if ((*head) -> prps == NULL || (*head) -> arg == NULL)
            MallocErr("AddSmnt 2");

        ((*head) -> prps)[0] = prps[0];
        ((*head) -> arg)[0] = arg[0];
        ((*head) -> prps)[1] = '\0';
        ((*head) -> arg)[1] = '\0';
    }
}

void DepthFirstTraversal(PrpsTree **clause, FindList **list)
{
    /*
     */
    PrpsTree **left, **right;
    
    if (clause != NULL)
    {   if ((*clause) != NULL)
        {   if (OprtrNodeType(clause, IS, OR))
            {   left = &((*clause) -> left);
                right = &((*clause) -> right);

                DepthFirstTraversal(left, list);
                DepthFirstTraversal(right, list);
            }
            else if (PrpsNode(clause))
                AddStmnt(list, (*clause) -> stmnt -> stc, (*clause) -> argmnt -> stc, *((*clause) -> neg));
        }
    }
}

int DefaultsToTrue(FindList **list)
{
    int return_val = 0;

    FindList **head = list;

    if (list != NULL)
        while ((*head) != NULL)
        {
            if ((*list) -> is && (*list) -> is_not)
            {
                return_val = 1;
                break;
            }

            head = &((*head) -> next);
        }

    return return_val;
}

/*FindList ***GetLiterals(PrpsTree **tree, FindList ***clause_array, int *k)
{
    FindList **list = NULL, **head = NULL;
    int left, right;
    FindList **return_val;

    if (OprtrNodeType(tree, IS, AND))
    {
        GetLiterals(&((*tree) -> left), clause_array, k);
        GetLiterals(&((*tree) -> right), clause_array, k);
    }

    if (OprtrNodeType(tree, IS, OR))
    {
        list = calloc(1, sizeof(FindList *));
        if (list == NULL)
            MallocErr("GetLiterals 0");

        DepthFirstTraversal(tree, list);
        return_val = DefaultsToTrue(list);
    }

    else
        InconsistencyErr("GetLiterals 1");

    return list;
}*/

int CountLiterals(PrpsTree **tree)
{
    int return_val = 0;

    if (OprtrNode(tree))
    {
        return_val += CountLiterals(&((*tree) -> left));
        return_val += CountLiterals(&((*tree) -> right));
    }
    else if (PrpsNode(tree))
        return_val = 1;

    return return_val;
}

void AddLiterals(PrpsTree **sub_tree, ClauseList **head, int *index)
{
    /*  Adds all the symbols of all literals in the clause which 
     *  PrpsTree **sub_tree is a subtree of, to the clause_array 
     *  array of head
     */

    char neg = ' ';
    if (OprtrNodeType(sub_tree, IS, OR))
    {
        AddLiterals(&((*sub_tree) -> left), head, index);
        AddLiterals(&((*sub_tree) -> right), head, index);
    }

    else if (PrpsNode(sub_tree))
    {
        ((*head) -> clause_array)[*index] = calloc(3, sizeof(char *));

        if (((*head) -> clause_array)[*index] == NULL)
            MallocErr("AddLiterals 1");

        if (*((*sub_tree) -> neg) == 1)
            neg = '~';

        ((*head) -> clause_array)[*index][0] = calloc(2, sizeof(char));
        if (((*head) -> clause_array)[*index][0] == NULL)
            MallocErr("AddLiterals 2");

        ((*head) -> clause_array)[*index][0][0] = neg;
        ((*head) -> clause_array)[*index][0][1] = '\0';
        ((*head) -> clause_array)[*index][1] = (*sub_tree) -> stmnt -> stc;
        ((*head) -> clause_array)[*index][2] = (*sub_tree) -> argmnt -> stc;
        *index += 1;
    }
}

void GetClauses(PrpsTree **tree, ClauseList **head, ClauseList **tail, int *index, int num_literals)
{
    /*
        Produces a linked list of subtrees, each of which is a clause
        The ClauseList ** is the linked list
    */

    int inc_array = 0;
    ClauseList **tmp = NULL;
    if (OprtrNode(tree) || PrpsNode(tree))
    {
        if (OprtrNodeType(tree, IS, AND))
        {   GetClauses(&((*tree) -> left), head, tail, index, num_literals);
            GetClauses(&((*tree) -> right), head, tail, index, num_literals);
        }

        else if (OprtrNodeType(tree, IS, OR) || PrpsNode(tree))
        {   
            if ((*head) == NULL)
            {   
                (*head) = calloc(1, sizeof(ClauseList));
                if ((*head) == NULL)
                    MallocErr("GetClauses 2");
                (*tail) = (*head);
            }

            else
            {
                (*head) -> next = calloc(1, sizeof(ClauseList));
                if ((*head) -> next == NULL)
                    MallocErr("GetClauses 3");
                (*head) = (*head) -> next;
            }

            num_literals = CountLiterals(tree);
            ((*head) -> clause_array) = calloc(num_literals, sizeof(char **));
            (*head) -> array_len = num_literals;
            AddLiterals(tree, head, index);
            *index = 0;
        }
        else
            InconsistencyErr("GetClauses 3");
    }
}

void print_clauses(ClauseList *clause_list)
{
    int i;
    while (clause_list != NULL)
    {   for (i = 0; i < clause_list -> array_len; i++)
        {   if ((clause_list -> clause_array)[i][0] == NULL)
                break;

            printf("%c%c[%c] ", (clause_list -> clause_array)[i][0][0], 
                (clause_list -> clause_array)[i][1][0],
                (clause_list -> clause_array)[i][2][0]);
        }

        printf("\n");
        clause_list = clause_list -> next;
    }
}

PL_Resolve(char ***Pa, char ***Pb)
{
    
}

int Resolve(ClauseList **clause_list)
{
    /* Resolves the clauses in clause_list. If 

     */

    int return_val = 0;

    ClauseList **ptr_tail = NULL, **ptr_head = NULL, **ptr_mid = NULL;

    ptr_tail = calloc(1, sizeof(ClauseList *));
    ptr_head = calloc(1, sizeof(ClauseList *));
    ptr_mid = calloc(1, sizeof(ClauseList *));

    if (!ptr_tail || !ptr_head || !ptr_mid)
        MallocErr("Resolve 1");

    *ptr_tail = *clause_list;
    *ptr_head = *clause_list;

    while ((*ptr_head) != NULL)
        (*ptr_head) = (*ptr_head) -> next;

    if ((*ptr_tail) != NULL)
        (*ptr_mid) = (*ptr_tail) -> next;

    if (
}

int Resolution(PrpsTree **tree)
{
    /*

    */

    int return_val, index = 0, num_literals = -1;
    ClauseList **clause_list_ptr = NULL;
    ClauseList **tail_ptr = NULL;

    clause_list_ptr = calloc(1, sizeof(ClauseList *));
    tail_ptr = calloc(1, sizeof(ClauseList *));
    if (clause_list_ptr == NULL || tail_ptr == NULL)
        MallocErr("PL_Resolution 1");

    GetClauses(tree, clause_list_ptr, tail_ptr, &index, num_literals);
    //(*clause_list_ptr) = (*tail_ptr);

    return_val = Resolve(tail_ptr);
    //print_clauses((*clause_list_ptr));

    return 0;
}