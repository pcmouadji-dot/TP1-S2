#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define true 1
#define false 0

/* ================================================================
   ANSI colour / style helpers
   ================================================================ */
   //remark:the color defines have been found in a github repository and not by us
#define RESET    "\033[0m"
#define BOLD     "\033[1m"
#define DIM      "\033[2m"

/* foreground colours */
#define FG_WHITE   "\033[97m"
#define FG_CYAN    "\033[96m"
#define FG_GREEN   "\033[92m"
#define FG_YELLOW  "\033[93m"
#define FG_RED     "\033[91m"
#define FG_BLUE    "\033[94m"
#define FG_MAGENTA "\033[95m"
#define FG_GRAY    "\033[90m"

/* background colours */
#define BG_BLUE    "\033[44m"
#define BG_DARK    "\033[40m"

/* -- Box-drawing constants (plain ASCII, works on all terminals) -- */
#define BOX_TL  "+"
#define BOX_TR  "+"
#define BOX_BL  "+"
#define BOX_BR  "+"
#define BOX_H   "="
#define BOX_V   "|"
#define BOX_ML  "+"
#define BOX_MR  "+"
#define BOX_TM  "+"
#define BOX_BM  "+"

#define LINE_H  "-"
#define LINE_TL "+"
#define LINE_TR "+"
#define LINE_BL "+"
#define LINE_BR "+"
#define LINE_V  "|"
#define LINE_ML "+"
#define LINE_MR "+"

#define BULLET  "  * "
#define ARROW   "  > "
#define CHECK   " [OK] "
#define CROSS   " [!!] "
#define STAR    " *** "
 
/* -- the ui----------------------------------------------- */
#define W 60   /* total box width (including borders) */

/* ================================================================
   basic funcs for the ui.
   ================================================================ */

/* print N copies of s */
void repeat(const char *s, int n) {
    for (int i = 0; i < n; ++i) printf("%s", s);
}

/* top border */
void boxTop(void) {
    printf(FG_BLUE BOX_TL); repeat(BOX_H, W-2); printf(BOX_TR RESET "\n");
}
/* divider line */
void boxMid(void) {
    printf(FG_BLUE BOX_ML); repeat(BOX_H, W-2); printf(BOX_MR RESET "\n");
}
/* bottom border */
void boxBot(void) {
    printf(FG_BLUE BOX_BL); repeat(BOX_H, W-2); printf(BOX_BR RESET "\n");
}
/* empty interior row */
void boxEmpty(void) {
    printf(FG_BLUE BOX_V RESET "%*s" FG_BLUE BOX_V RESET "\n", W-2, "");
}
/* centred text row - text must be plain ASCII width len */
void boxCentre(const char *colour, const char *text) {
    int len = (int)strlen(text);
    int pad = W - 2 - len;
    int left  = pad / 2;
    int right = pad - left;
    printf(FG_BLUE BOX_V RESET);
    printf("%*s", left, "");
    printf("%s%s" RESET, colour, text);
    printf("%*s", right, "");
    printf(FG_BLUE BOX_V RESET "\n");
}
/* left-aligned text row */
void boxLine(const char *colour, const char *text) {
    int len = (int)strlen(text);
    int right = W - 2 - len;
    if (right < 0) right = 0;
    printf(FG_BLUE BOX_V RESET);
    printf("%s%s" RESET, colour, text);
    printf("%*s", right, "");
    printf(FG_BLUE BOX_V RESET "\n");
}

/* thin separator (no box) */
void thinLine(void) {
    printf(FG_GRAY);
    repeat(LINE_H, W);
    printf(RESET "\n");
}

/* section label */
void sectionLabel(const char *label) {
    printf("\n" FG_CYAN BOLD "  %s" RESET "\n", label);
    printf(FG_GRAY "  ");
    repeat(LINE_H, (int)strlen(label) + 2);
    printf(RESET "\n");
}

/* -- Error / success / info banners --------------------------- */
 void printError(const char *msg) {
    printf(FG_RED BOLD CROSS "%s" RESET "\n", msg);
}
 void printSuccess(const char *msg) {
    printf(FG_GREEN BOLD CHECK "%s" RESET "\n", msg);
}
 void printInfo(const char *msg) {
    printf(FG_YELLOW BOLD ARROW "%s" RESET "\n", msg);
}

/*---------------------------------------------------------------*/
/*the tree declaration*/
typedef struct Node {
    char val[256];
    struct Node *left, *right;
} Node;

void NodeAllocate(Node **root, char val[256], int *codeError) {
    *root = (Node*)malloc(sizeof(Node));
    *codeError = (*root == NULL);
    if (!(*codeError)) {
        strncpy((*root)->val, val, 255);
        (*root)->val[255] = '\0';
        (*root)->left = NULL;
        (*root)->right = NULL;
    }
}
char *NodeValue(Node *root) {
    char *str = (char*)malloc(sizeof(char) * 256);
    strncpy(str, root->val, 255);
    str[255] = '\0';
    return str;
}
Node *LC(Node *root) { return root->left; }
Node *RC(Node *root) { return root->right; }
void AssNodeVal(Node *root, char val[256]) {
    strncpy(root->val, val, 255);
    root->val[255] = '\0';
}
void AssNodeLC(Node *root, Node *left)  { root->left  = left; }
void AssNodeRC(Node *root, Node *right) { root->right = right; }

/*------------------functions declaration ---------------------------------*/
int max(int a, int b);
int StrCMP(char strOne[256], char strTwo[256]);
int SpecialCharacter(char c);
void PrintTree(Node *root);
void SearchBST(Node *root, char val[256], Node **node, Node **parent);
void InsertBST(Node **root, char val[256]);
void PasteBST(Node *pasteFrom, Node **pasteTo);
void UniBST(Node *rootOne, Node *rootTwo, Node **rootUnion);
void InterBST(Node *rootOne, Node *rootTwo, Node **rootIntersection);
void SymDifBST(Node *rootOne, Node *rootTwo, Node **rootSymmetricalDifference);
void DifBST(Node *rootOne, Node *rootTwo, Node **rootDifference);
int fAdv(FILE *f, char *c);
void fParaBST(char *filePath, int prgNum, Node **root);
void DestroyBST(Node *root);

/*---------------------------------------------------*/

int countParagraphs(const char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) return -1;
    int count = 0, inBlock = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        int blank = 1, i = 0;
        while (line[i] != '\0') {
            if (line[i] != ' ' && line[i] != '\t' &&
                line[i] != '\r' && line[i] != '\n') { blank = 0; break; }
            i++;
        }
        if (!blank) { if (!inBlock) { count++; inBlock = 1; } }
        else inBlock = 0;
    }
    fclose(f);
    return count;
}

/* ================================================================
   desine
   ================================================================ */

/* -- Banner shown once at startup -- */
 void printBanner(void) {
    printf("\n");
    boxTop();
    boxEmpty();
    boxCentre(FG_CYAN BOLD, "ADDS Lab - Set Operations on Text");
    boxCentre(FG_WHITE BOLD, "Algorithmics & Dynamic Data Structures");
    boxEmpty();
    boxMid();
    boxCentre(FG_GRAY, "Binary Search Tree | Union | Intersection | Difference");
    boxEmpty();
    boxBot();
    printf("\n");
}

/* -- printed in top of the mean to say the current files working on  -- */
void printFileStatus(const char *pathA, int cntA,
                             const char *pathB, int cntB) {
    char buf[W];
    sectionLabel("Loaded Files");
    snprintf(buf, sizeof(buf), "  [A]  %s", pathA);
    buf[W-6] = '\0';   /* truncate long paths */
    printf(FG_WHITE "%s" RESET, buf);
    printf(FG_GREEN "  (%d paragraph%s)\n" RESET, cntA, cntA == 1 ? "" : "s");

    snprintf(buf, sizeof(buf), "  [B]  %s", pathB);
    buf[W-6] = '\0';
    printf(FG_WHITE "%s" RESET, buf);
    printf(FG_GREEN "  (%d paragraph%s)\n" RESET, cntB, cntB == 1 ? "" : "s");
}

/* -- Main-menu box -- */
void printMainMenu(void) {
    sectionLabel("Choose an Operation");
    printf(FG_CYAN "  1" RESET "  -  Union          " FG_GRAY "(A u B)\n" RESET);
    printf(FG_CYAN "  2" RESET "  -  Intersection   " FG_GRAY "(A n B)\n" RESET);
    printf(FG_CYAN "  3" RESET "  -  Sym. Difference " FG_GRAY "(A delta B)\n" RESET);
    printf(FG_CYAN "  4" RESET "  -  Difference A\\B  " FG_GRAY "(A \\ B)\n" RESET);
    printf(FG_YELLOW "  5" RESET "  -  Load new files\n");
    printf(FG_RED "  6" RESET "  -  Exit\n");
    thinLine();
    printf(FG_WHITE BOLD "  Your choice: " RESET);
}

/* -- Scope-menu box -- */
void printScopeMenu(void) {
    sectionLabel("Select Scope");
    printf(FG_CYAN "  1" RESET "  -  Same file           "
           FG_GRAY "(two paragraphs from File A)\n" RESET);
    printf(FG_CYAN "  2" RESET "  -  Cross file once      "
           FG_GRAY "(one paragraph from each file)\n" RESET);
    printf(FG_CYAN "  3" RESET "  -  Consecutive chain    "
           FG_GRAY "(chain results with paragraphs from B)\n" RESET);
    thinLine();
    printf(FG_WHITE BOLD "  Your choice: " RESET);
}

/* -- Operation label -- */
const char *opName(int op) {
    if (op == 1) return "Union  (A u B)";
    if (op == 2) return "Intersection  (A n B)";
    if (op == 3) return "Sym. Difference  (A delta B)";
    return "Difference A\\B  (A \\ B)";
}

/* -- Result header / footer -- */
void printResultHeader(int op) {
    printf("\n");
    thinLine();
    printf(FG_MAGENTA BOLD "  >>  Running: %s\n" RESET, opName(op));
    thinLine();
}
void printResultFooter(const char *note) {
    thinLine();
    printf(FG_CYAN BOLD "  Result (alphabetical order):\n" RESET);
    printf(FG_GRAY "  ");
    repeat(LINE_H, W - 4);
    printf(RESET "\n");
}
void printResultNote(const char *note) {
    printf(FG_GRAY "\n  %s\n" RESET, note);
    thinLine();
    printf("\n");
}

/* -- Goodbye statement in ther end - */
void printGoodbye(void) {
    printf("\n");
    boxTop();
    boxEmpty();
    boxCentre(FG_CYAN BOLD, "Thank you for using ADDS Lab!");
    boxCentre(FG_GRAY, "Goodbye  -  Have a great day");
    boxEmpty();
    boxBot();
    printf("\n");
}

/* -------------------------------------------------------------
   loading the files to the code
   ------------------------------------------------------------- */

int loadFilePath(char *pathOut, char fileLabel) {
    int count;
    while (1) {
        printf(FG_WHITE BOLD "  Path for File %c: " RESET, fileLabel);
        scanf("%255s", pathOut);
        count = countParagraphs(pathOut);
        if (count == -1) {
            printError(" Cannot open that file - check the path and try again.");
            printf("\n");
        } else if (count == 0) {
            printError(" File has no paragraphs - please choose another file.");
            printf("\n");
        } else {
            char buf[128];
            snprintf(buf, sizeof(buf), " File accepted - %d paragraph%s found.",
                     count, count == 1 ? "" : "s");
            printSuccess(buf);
            printf("\n");
            return count;
        }
    }
}

int readInt(void) {//to avoid the complex intering everytime.
    int x;
    if (scanf("%d", &x) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        return -1;
    }
    return x;
}

int chooseParagraph(char label, int maxCount) {
    int choice;
    while (1) {
        printf(FG_WHITE BOLD "  Paragraph %c (1 - %d): " RESET, label, maxCount);
        choice = readInt();
        if (choice >= 1 && choice <= maxCount) return choice;
        char buf[128];
        snprintf(buf, sizeof(buf),
                 " Invalid - enter a number between 1 and %d.", maxCount);
        printError(buf);
    }
}

/* -------------------------------------------------------------
   Run an operation, pretty-print result, return result tree
   ------------------------------------------------------------- */
Node *runOperation(int operation, Node *treeA, Node *treeB) {
    Node *result = NULL;
    printResultHeader(operation);

    if      (operation == 1) UniBST   (treeA, treeB, &result);
    else if (operation == 2) InterBST (treeA, treeB, &result);
    else if (operation == 3) SymDifBST(treeA, treeB, &result);
    else                     DifBST   (treeA, treeB, &result);

    printResultFooter(NULL);
    PrintTree(result);
    return result;
}

/* ===============================================================
   main
   =============================================================== */
int main(void) {
    int  totalParagraphsFileA, totalParagraphsFileB;
    char pathA[256], pathB[256];
    int  operation, scope, pA, pB;

    printBanner();

    /* -- load files once before the main loop -- */
    sectionLabel("Load Input Files");
    totalParagraphsFileA = loadFilePath(pathA, 'A');
    totalParagraphsFileB = loadFilePath(pathB, 'B');

    while (1) {

        /* -- STEP 1: main menu -- */
        printFileStatus(pathA, totalParagraphsFileA, pathB, totalParagraphsFileB);
        printMainMenu();
        operation = readInt();

        if (operation == 6) { printGoodbye(); break; }

        if (operation == 5) {
            sectionLabel("Load New Files");
            totalParagraphsFileA = loadFilePath(pathA, 'A');
            totalParagraphsFileB = loadFilePath(pathB, 'B');
            continue;
        }

        if (operation < 1 || operation > 4) {
            printError(" Please enter 1, 2, 3, 4, 5, or 6.");
            continue;
        }

        /* -- STEP 2: scope menu -- */
        printScopeMenu();
        scope = readInt();

        if (scope < 1 || scope > 3) {
            printError(" Please enter 1, 2, or 3.");
            continue;
        }

        /* ==========================================
           part1 - Same file (two paragraphs from A)
           ========================================== */
        if (scope == 1) {
            if (totalParagraphsFileA < 2) {
                printError(" File A has only 1 paragraph - need at least 2.");
                continue;
            }
            sectionLabel("Select 2 Paragraphs from File A");
            printf("  Paragraph #1\n");
            pA = chooseParagraph('A', totalParagraphsFileA);
            printf("  Paragraph #2\n");
            pB = chooseParagraph('A', totalParagraphsFileA);

            Node *treeA = NULL, *treeB = NULL;
            fParaBST(pathA, pA, &treeA);
            fParaBST(pathA, pB, &treeB);

            Node *result = runOperation(operation, treeA, treeB);

            char note[128];
            snprintf(note, sizeof(note),
                     "Paragraphs %d and %d from File A", pA, pB);
            printResultNote(note);

            DestroyBST(treeA);
            DestroyBST(treeB);
            DestroyBST(result);
        }

        /* ==========================================
           parte2 - Cross file once
           ========================================== */
        else if (scope == 2) {
            sectionLabel("Select One Paragraph from Each File");
            printf(FG_YELLOW "  -- File A: %s --\n" RESET, pathA);
            pA = chooseParagraph('A', totalParagraphsFileA);
            printf(FG_YELLOW "\n  -- File B: %s --\n" RESET, pathB);
            pB = chooseParagraph('B', totalParagraphsFileB);

            Node *treeA = NULL, *treeB = NULL;
            fParaBST(pathA, pA, &treeA);
            fParaBST(pathB, pB, &treeB);

            Node *result = runOperation(operation, treeA, treeB);

            char note[256];
            snprintf(note, sizeof(note),
                     "Paragraph %d from A  |  Paragraph %d from B", pA, pB);
            printResultNote(note);

            DestroyBST(treeA);
            DestroyBST(treeB);
            DestroyBST(result);
        }

        /* ==========================================
           part3 - Consecutive chain
           ========================================== */
        else {
            sectionLabel("Chain - Initialise");
            printf(FG_YELLOW "  -- File A: %s --\n" RESET, pathA);
            pA = chooseParagraph('A', totalParagraphsFileA);
            printf(FG_YELLOW "\n  -- File B: %s --\n" RESET, pathB);
            pB = chooseParagraph('B', totalParagraphsFileB);

            Node *chainResult = NULL, *treeA = NULL, *treeB = NULL;
            fParaBST(pathA, pA, &treeA);
            fParaBST(pathB, pB, &treeB);

            chainResult = runOperation(operation, treeA, treeB);

            char note[256];
            snprintf(note, sizeof(note),
                     "Paragraph %d from A  |  Paragraph %d from B", pA, pB);
            printResultNote(note);

            DestroyBST(treeA);
            DestroyBST(treeB);
            treeA = NULL; treeB = NULL;

            /* -- chain loop -- */
            while (1) {
                sectionLabel("Chain - Continue");
                printf(FG_CYAN "  1" RESET "  -  Union        "
                       FG_GRAY "(result u new paragraph)\n" RESET);
                printf(FG_CYAN "  2" RESET "  -  Intersection "
                       FG_GRAY "(result n new paragraph)\n" RESET);
                printf(FG_CYAN "  3" RESET "  -  Sym. Diff    "
                       FG_GRAY "(result delta new paragraph)\n" RESET);
                printf(FG_CYAN "  4" RESET "  -  Difference   "
                       FG_GRAY "(result \\ new paragraph)\n" RESET);
                printf(FG_RED  "  5" RESET "  -  Stop chain and return to menu\n");
                thinLine();
                printf(FG_WHITE BOLD "  Your choice: " RESET);
                operation = readInt();

                if (operation == 5) {
                    printInfo(" Chain stopped - returning to main menu.");
                    break;
                }
                if (operation < 1 || operation > 4) {
                    printError(" Please enter 1, 2, 3, 4, or 5.");
                    continue;
                }

                char chainPath[256];
                sectionLabel("Load Next File for Chain");
                int chainTotal = loadFilePath(chainPath, 'N');
                printf(FG_YELLOW "  -- File: %s --\n" RESET, chainPath);
                pB = chooseParagraph('N', chainTotal);

                fParaBST(chainPath, pB, &treeB);

                Node *newResult = runOperation(operation, chainResult, treeB);
                snprintf(note, sizeof(note),
                         "Previous result  op  paragraph %d from '%s'",
                         pB, chainPath);
                printResultNote(note);

                DestroyBST(chainResult);
                DestroyBST(treeB);
                treeB = NULL;

                chainResult = newResult;
            }

            /* final chain result */
            printf("\n");
            thinLine();
            printf(FG_MAGENTA BOLD "  ***  Final Chain Result (alphabetical order):\n" RESET);
            thinLine();
            PrintTree(chainResult);
            thinLine();
            printf("\n");
            DestroyBST(chainResult);
        }
    }

    return 0;
}

/* ================================================================
   the function section :
   ================================================================ */

int max(int a, int b) { return a >= b ? a : b; }

int StrCMP(char strOne[256], char strTwo[256]) {
    int i = 0, len = max(strlen(strOne), strlen(strTwo));
    while (strOne[i] == strTwo[i] && i < len) ++i;
    return (int)strOne[i] - (int)strTwo[i];
}

int SpecialCharacter(char c) {
    return ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z') && (c != 39));
}

void PrintTree(Node *root) {
    if (root != NULL) {
        PrintTree(LC(root));
        printf("  " FG_WHITE "%s\n" RESET, NodeValue(root));
        PrintTree(RC(root));
    }
}

void SearchBST(Node *root, char val[256], Node **node, Node **parent) {
    int cmpResult;
    if (root == NULL) { *node = NULL; *parent = NULL; }
    else {
        cmpResult = StrCMP(NodeValue(root), val);
        if      (cmpResult > 0) SearchBST(LC(root), val, node, parent);
        else if (cmpResult < 0) SearchBST(RC(root), val, node, parent);
        else { *node = root; *parent = NULL; return; }
        if (*parent == NULL) *parent = root;
    }
}

void InsertBST(Node **root, char val[256]) {
    Node *node, *parent;
    int cmpResult, codeError;
    if (*root == NULL) {
        NodeAllocate(root, val, &codeError);
        if (codeError) *root = NULL;
        return;
    }
    SearchBST(*root, val, &node, &parent);
    if (node == NULL) {
        NodeAllocate(&node, val, &codeError);
        if (!codeError) {
            cmpResult = StrCMP(NodeValue(parent), val);
            if (cmpResult > 0) AssNodeLC(parent, node);
            else               AssNodeRC(parent, node);
        }
    }
}

void PasteBST(Node *pasteFrom, Node **pasteTo) {
    if (pasteFrom != NULL) {
        InsertBST(pasteTo, NodeValue(pasteFrom));
        PasteBST(LC(pasteFrom), pasteTo);
        PasteBST(RC(pasteFrom), pasteTo);
    }
}

void Union(Node *rootOne, Node *rootTwo, Node **rootUnion) {
    if (*rootUnion == NULL) PasteBST(rootTwo, rootUnion);
    if (rootOne != NULL) {
        Union(LC(rootOne), rootTwo, rootUnion);
        InsertBST(rootUnion, NodeValue(rootOne));
        Union(RC(rootOne), rootTwo, rootUnion);
    }
}

void Intersection(Node *rootOne, Node *rootTwo, Node **rootIntersection) {
    Node *node, *parent;
    if (rootOne != NULL && rootTwo != NULL) {
        SearchBST(rootOne, NodeValue(rootTwo), &node, &parent);
        if (node != NULL) InsertBST(rootIntersection, NodeValue(rootTwo));
        Intersection(rootOne, LC(rootTwo), rootIntersection);
        Intersection(rootOne, RC(rootTwo), rootIntersection);
    }
}

void HalfSymmetricalDifference(Node *rootOne, Node *rootTwo, Node **rootResult) {
    Node *node, *parent;
    if (rootOne != NULL) {
        SearchBST(rootTwo, NodeValue(rootOne), &node, &parent);
        if (node == NULL) InsertBST(rootResult, NodeValue(rootOne));
        HalfSymmetricalDifference(LC(rootOne), rootTwo, rootResult);
        HalfSymmetricalDifference(RC(rootOne), rootTwo, rootResult);
    }
}

void UniBST(Node *rootOne, Node *rootTwo, Node **rootUnion) {
    *rootUnion = NULL;
    Union(rootOne, rootTwo, rootUnion);
}

void InterBST(Node *rootOne, Node *rootTwo, Node **rootIntersection) {
    *rootIntersection = NULL;
    Intersection(rootOne, rootTwo, rootIntersection);
}

void SymDifBST(Node *rootOne, Node *rootTwo, Node **rootSymmetricalDifference) {
    *rootSymmetricalDifference = NULL;
    HalfSymmetricalDifference(rootOne, rootTwo, rootSymmetricalDifference);
    HalfSymmetricalDifference(rootTwo, rootOne, rootSymmetricalDifference);
}

/* simple A\B difference - words in A that are not in B */
void DifBST(Node *rootOne, Node *rootTwo, Node **rootDifference) {
    *rootDifference = NULL;
    HalfSymmetricalDifference(rootOne, rootTwo, rootDifference);
}

int fAdv(FILE *f, char *c) { *c = fgetc(f); return (*c != EOF); }

void fParaBST(char *filePath, int prgNum, Node **root) {
    FILE *f = fopen(filePath, "r");
    if (!f) return;
    char line[1024];
    int currentPara = 0, inBlock = 0;
    *root = NULL;
    while (fgets(line, sizeof(line), f)) {
        int isBlank = 1;
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] != ' ' && line[i] != '\t' &&
                line[i] != '\n' && line[i] != '\r') { isBlank = 0; break; }
        }
        if (!isBlank) {
            if (!inBlock) { currentPara++; inBlock = 1; }
            if (currentPara == prgNum) {
                char word[256];
                int wIdx = 0;
                for (int i = 0; line[i] != '\0'; i++) {
                    if (!SpecialCharacter(line[i])) {
                        word[wIdx++] = line[i];
                    } else if (wIdx > 0) {
                        word[wIdx] = '\0';
                        InsertBST(root, word);
                        wIdx = 0;
                    }
                }
                if (wIdx > 0) { word[wIdx] = '\0'; InsertBST(root, word); }
            }
        } else {
            inBlock = 0;
            if (currentPara == prgNum) break;
        }
    }
    fclose(f);
}

void DestroyBST(Node *root) {
    if (root != NULL) {
        DestroyBST(LC(root));
        DestroyBST(RC(root));
        free(root);
    }
}