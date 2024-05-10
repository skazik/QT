#ifdef MAIN_PREVIOUS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llist.h"

#ifndef NULL
#define NULL (0)
#endif

#if 1
int main()
{
    printf("hello. it started\n");
    return 0;
}


#elif 1

int main()
{
    char str[32] = "1234567890qwertyuiop";

    int i = 0;
    int len = strlen(str);
    char *st = &str[len];

    printf("origin: [%s]\n", str);
    for (; i < len/2; i++)
    {
        *st = str[i];
        str[i] = str[len-1-i];
        str[len-1-i] = *st;
    }
    *st = '\0';
    printf("transf: [%s]\n", str);
    return 0;
}

#elif 1

int main()
{
    // lltest();

    llist_t* a = newlist();
    llist_t* b = newlist();

    insertatbegin(a, 1);
    insertatend(a, 5);
    insertatend(a, 9);

    insertatbegin(b, 2);
    insertatend(b, 4);
    insertatend(b, 6);

    printList(a);
    printList(b);

    llist_t* c = newlist();

    struct node* aa = a->head;
    struct node* bb = b->head;

    while ((aa != NULL) || (bb != NULL))
    {
        struct node** tmp = &bb;
        if (aa != NULL && ((bb == NULL) || (aa->data <= bb->data)))
        {
            tmp = &aa;
        }
        insertatend(c, (*tmp)->data);
        *tmp = (*tmp)->next;
    }
    printList(c);
    // release memory for a, b, c
    return 0;
}

#elif 1
typedef struct _LinkList
{
    int value;
    struct _LinkList * next;
    struct _LinkList * pTail;
} LinkList_t;

void llInit(LinkList_t *pList, int value)
{
    pList->value = value;
    pList->next = NULL;
    pList->pTail = pList;
}

int llAdd(LinkList_t *pList, int value)
{
    if (pList->pTail == NULL)
    {
        pList->pTail = malloc(sizeof(LinkList_t));
        if (pList->pTail == NULL)
        {
            printf("memory allocation problem\n");
            return -1;
        }
        pList->pTail->value = value;
        pList->pTail->next = NULL;
        pList->pTail = pList->pTail->next;
    }
    else if (pList->pTail->next == NULL)
    {
        if (NULL == (pList->pTail->next = malloc(sizeof(LinkList_t))))
        {
            printf("memory allocation problem\n");
            return -2;
        }
        pList->pTail = pList->pTail->next;
        pList->pTail->value = value;
        pList->pTail->next = NULL;
    }
    return 0;
}

int main()
{
    LinkList_t a = {0, NULL, NULL};
    LinkList_t b = {0, NULL, NULL};
    LinkList_t c = {0, NULL, NULL};

    llInit(&a, 1);
    llAdd(&a, 3);
    llAdd(&a, 5);

    llInit(&b, 2);
    llAdd(&b, 4);
    llAdd(&b, 6);

    llInit(&c, 0);

    a.pTail = &a;
    b.pTail = &b;
    c.pTail = &c;

    while ((a.pTail != NULL) || (b.pTail != NULL))
    {
        if (a.pTail != NULL && a.pTail->value <= b.pTail->value)
        {
            llAdd(&c, a.pTail->value);
            a.pTail = a.pTail->next;
        }
        else if (b.pTail != NULL)
        {
            llAdd(&c, b.pTail->value);
            b.pTail = b.pTail->next;
        }
    }

    c.pTail = c.next;
    while(c.pTail != NULL)
    {
        printf("%d, ", c.pTail->value);
        c.pTail = c.pTail->next;
    }
    printf("\n");
    return 0;
}

#elif 1
long long getMaxAdditionalDinersCount(long long N, long long K, int M, long long* S) {

  int i = 1;
  int count = 0;
  unsigned char mask[N+1+K];
  memset(mask, 0, N+1+K);

  for (; i <= N; i++)
  {
    int j = 0;
    for (; j < M; j++)
    {
      if (i == S[j])
      {
        mask[i] = 1;
        break;
      }
    }
    printf("%d ", mask[i]);
  }
  printf("\n");

  for (i = 1; i <= N; i++)
  {
    int k = 0;
    for (k = (i - K) < 1 ? 1 : (i - K); (k <= i+K); k++)
    {
      if (mask[k] != 0)
      {
         break;
      }
    }
    if ((k == i+K+1)) //  || ( k == N+1))
    {
      count++;
      mask[i] = 1;
    }
    printf("%d ", mask[i]);
  }
  printf("\n");

  return count;
}
int main ()
{
    long long s[] = {2, 6};
    long long d[] = {11, 6, 14};
    int count = getMaxAdditionalDinersCount(10, 1, 2, s);
    printf("count = %d\n", count);
    count = getMaxAdditionalDinersCount(15, 2, 3, d);
    printf("count = %d\n", count);
    return 0;
}
#elif 1
typedef struct {
    int val;
    int count;
} Kids_t;

int search_and_add(Kids_t * pKidArr, int kid, Kids_t * pParArr, int parent, int arrLength)
{
    int i = 0;
    for (; i < arrLength; i++)
    {
        Kids_t pPar = pParArr[i];
        if ((pPar.val == parent) || (pPar.val == 0))
        {
            if (pPar.count < 2)
            {
                pParArr[i].count++;
                pParArr[i].val = parent;
                break;
            }
            else
            {
                return 0;
            }
        }
    }
    for (i=0; i < arrLength; i++)
    {
        Kids_t pKid = pKidArr[i];
        if ((pKid.val == kid) || (pKid.val == 0))
        {
            if (pKid.count < 1)
            {
                pKidArr[i].count++;
                pKidArr[i].val = kid;
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    return 0;
}
void TreeConstructor(char * strArr[], int arrLength) {

    int i = 0;
    // lookup table
    Kids_t *kids = malloc(arrLength * sizeof(Kids_t));
    Kids_t prts[arrLength * sizeof(Kids_t)];

    memset(kids, 0, arrLength * sizeof(Kids_t));
    memset(prts, 0, arrLength * sizeof(Kids_t));


    for (; i < arrLength; i++)
    {
        int kid = 0;
        int parent = 0;
        char * st = strArr[i];
        sscanf(st, "(%d,%d)", &kid, &parent);
        if (!search_and_add(kids, kid, prts, parent, arrLength))
        {
            printf("%s\n", "false");
            free(kids);
            return;
        }
    }
    printf("%s\n", "true");
    free(kids);
}

int main(void) {

  // keep this function call here
    char * A[] = {"(1,2)", "(2,4)", "(5,7)", "(7,2)", "(9,5)", "(1,7)"};
    char * B[] = {"(1,2)", "(3,2)", "(2,12)", "(5,2)"};
  int arrLength = sizeof(A) / sizeof(*A);
  int bLen =  sizeof(B) / sizeof(*B);
  TreeConstructor(A, arrLength);
  TreeConstructor(B, bLen);
  return 0;

}

#elif 1
#define LETTER(sen) (((sen >= 'a') && (sen <= 'z')) || ((sen >= 'A') && (sen <= 'Z')))
#define NUMBER(_val) ((_val > 48) && (_val < 58))

void CodelandUsernameValidation(char * str) {


  if ((strlen(str) < 4) || (strlen(str) > 25) ||
      (str[strlen(str)-1] == '_') || !(LETTER(str[0])))
      {
        printf("%s\n", "false");
        return;
      }

    int i = 0;
    for (; i < (int) strlen(str); i++)
    {
      if (!LETTER(str[i]) && !NUMBER(str[i]) && (str[i] != '_')) {
        printf("%s\n", "false");
        return;
      }
    }

  printf("%s\n", "true");

}

int main(void) {

  // keep this function call here
    CodelandUsernameValidation("aa_");
    CodelandUsernameValidation("aa_dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd");
    CodelandUsernameValidation("____aa_");
    CodelandUsernameValidation("u__hello_world123");
  return 0;

}
#elif 1
void LongestWord(char * sen) {

  int i, max, count = 0;
  char *out = strdup(sen);

  for (i = 0, max = 0; i < (int) strlen(sen); i++)
  {
    if (((sen[i] >= 'a') && (sen[i] <= 'z')) ||
        ((sen[i] >= 'A') && (sen[i] <= 'Z')))
        {
          count++;
          continue;
        }

    if (count && (count > max))
      {
        strncpy(out, &sen[i-count], count);
        out[count]='\0';
        max = count;
      }
    count = 0;

  }
  if (count && (count > max))
  {
    strncpy(out, &sen[i-count], count);
    out[count]='\0';
    count = 0;
  }

  printf("%s\n", out);
  free(out);

}
int main(void) {

  // keep this function call here
    LongestWord("fun&!! time");
    LongestWord("I love dogs and cats");
  return 0;

}
#elif 1
void FirstReverse(char * str) {

  int i = 0;
  int len= strlen(str);
  char *out = strdup(str);
  for (; i < len/2; i++)
  {
      out[len] = out[i];
      out[i] = out[len-i-1];
      out[len-i-1] = out[len];
  }
  out[len] = '\0';
  printf("%s\n", out);

}

int main(void) {

  // keep this function call here
  FirstReverse("I Love Code");
  return 0;

}
#elif 1
#define NUMBER(_val) ((_val > 48) && (_val < 58))
void QuestionsMarks(char * str) {

  int i, qcount;
  char first = 0;
  char second = 0;
  char found_any = 0;

  for(i=0, qcount = 0; i < (int) strlen(str); i++)
  {
      if (NUMBER(str[i]))
      {
          if (first == 0)
          {
              first = str[i];
              qcount = 0;
          }
          else if (second == 0)
          {
              second = str[i];
          }

          if (first - 48 + second - 48 == 10)
          {
              if (qcount != 3)
              {
                  printf("%s\n", "false");
                  return;
              }
              found_any = 1;
          }

          if (first && second)
          {
              qcount = 0;
              first = second;
              second = 0;
          }
      }
      else if (str[i] == '?')
      {
          qcount++;
      }
  }

  printf("%s\n", found_any ? "true":"false");

}

int main(void) {

  // keep this function call here
  QuestionsMarks("aa6?9");
  QuestionsMarks("acc?7??sss?3rr1??????5");
  return 0;
}

#elif 1
int validate(char *mask, int len)
{
  int i = 0;
  for (; i < len; i++)
  {
    if (mask[i] != '1')
      return 0;
  }
  return 1;
}

void MinWindowSubstring(char * strArr[], int arrLength) {

  // code goes here
  int i, j, min, len = strlen(strArr[0]);
  char *out = (char*) malloc(len+1);
  int k, mask_len = strlen(strArr[1]);
  char *mask = (char*) malloc(mask_len);
  char *pChar = NULL;

  if (!out || !mask)
  {
    printf("mem alloc failed, abort\n");
    return;
  }

  for (i = 0, min = len; i < len; i++)
  {
    memset(mask, '0', mask_len);
    for (j = i; j < len; j++)
    {
      if ((pChar = strchr(strArr[1], strArr[0][j])))
      {
        while (pChar)
        {
            if (mask[pChar-strArr[1]] == '0')
            {
                mask[pChar-strArr[1]] = '1';
                break;
            }
            if (pChar < strArr[1] + mask_len)
                pChar = strchr(pChar+1, strArr[0][j]);
            else
                break;
        };


        if (validate(mask, mask_len) && (j-i+1 < min))
        {
          min = j-i+1;
          strncpy(out, &strArr[0][i], min);
          out[min]='\0';
          break;
        }
      }
    }
  }

  printf("%s\n", out);
  free(out);
  free(mask);

}
int main(void) {

  // keep this function call here
    char * A[] = {"ahffaksfajeeubsne", "jefaa"};
    char * B[] = {"aaffhkksemckelloe", "fhea"};
  int arrLength = sizeof(A) / sizeof(*A);
  MinWindowSubstring(A, arrLength);
  MinWindowSubstring(B, arrLength);
  return 0;

}
// n element value 1.. n
// 1 duplicaton
#elif 1
void FindIntersection(char * strArr[], int arrLength)
{
  // printf("%s, and %s\n", strArr[0], strArr[1]);
  char*pArr = strdup(strArr[0]);

  if (arrLength < 1)
  {
    printf("wrong argument lenght %d, exit\n", arrLength);
    return;
  }

  char *pStart = pArr;
  //printf("pStart: %s\n", pStart);

  char *pEnd = strchr(pArr, ',');
  //printf("pEnd: %s\n", pEnd);

  while (pStart != NULL)
  {
    char pTmp = '\0';
    if (pEnd != NULL) {
      pTmp = *pEnd;
      *pEnd = '\0';
    }

    if (strstr(strArr[1], pStart)) {
      if (*pStart == ' ')
        printf(",%s", pStart + 1);
      else
        printf("%s", pStart);
    }

    if (pTmp)
        *pEnd = pTmp;


    if (!pEnd)
        break;
    pStart = pEnd + 1;
    pEnd = strchr(pStart, ',');

  }
  free(pArr);
  printf("\n");

}
int main(void) {

  // keep this function call here
  char * A[] = {"1, 3, 4, 7, 13", "1, 2, 4, 13, 15"};
  char * B[] = {"1, 3, 9, 10, 17, 18", "1, 4, 9, 10"};
  FindIntersection(A, sizeof(A) / sizeof(*A));
  FindIntersection(B, sizeof(B) / sizeof(*B));
  return 0;

}
#elif 0
int main()
{
    unsigned char arr[] = {7,5,0,4,2};
    unsigned int sz =  (unsigned int) sizeof(arr)/sizeof(char);

    printf("origin: ");
    for (unsigned int i=0; i < sz; i++)
        printf("%d, ", arr[i]);
    printf("\n");

    for (unsigned int i = 0; i < sz/2; i++)
    {
        int tmp = arr[i];
        arr[i] = arr[sz-i-1];
        arr[sz-i-1] = tmp;
    }
    printf("result: ");
    for (unsigned int i=0; i < sz; i++)
        printf("%d, ", arr[i]);
    printf("\n");
    return 0;
}

#elif 0

/**
 * @brief substituteDeprecatedTimezone
 * Convert deprecated timezone value to updated
 * 
 * @param country
 * @param timezone
 * @return replaced timezone or original if not found
 */
substituteDeprecatedTimezone(country: String?, timezone: String?) : String? {
        class DeprecatedTimezone(
            val country: String,
            val deprecatedTimezone: String,
            val substituteTimezone: String,
        )

        val deprecatedTimezones = listOf(
            DeprecatedTimezone("New Zealand", "Antarctica/South_Pole", "Pacific/Auckland")
        )

        return deprecatedTimezones.find { it.country == country && it.deprecatedTimezone == timezone }?.substituteTimezone ?: timezone
    }

#elif 0

int main()
{
    unsigned char arr[] =
    {   1,2,3,
        4,5,6,
        7,8,9,
    };
    int cols= 3;
    int rows = 3;

    printf("input:");
    for (int i = 0; i < cols*rows; i++)
    {
        printf(" %d,", arr[i]);
    }
    printf("\n");

    // transcoding

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < i; j++) {
            int tmp = arr[i*rows+j];
            arr[i*rows+j] = arr[j*cols+i];
            arr[j*cols+i] = tmp;
        }
    }
    printf("output:");
    for (int i = 0; i < cols*rows; i++)
    {
        printf(" %d,", arr[i]);
    }
    printf("\n");


    return 0;
}











#elif 1
// skyline
typedef struct _point
{
    int x,y;
} Point_t;

typedef struct _house
{
    Point_t p1; // left upper
    Point_t p2; // right bottom
} House_t;

#define LIST_MAX 6

House_t hList[LIST_MAX * 2] =
{
    {{0,5},{5,0}},
    {{5,7},{9,2}},
    {{8,15},{12,2}},
    {{8,5},{14,0}},
    {{13,8},{15,0}},
    {{17,5},{20,0}}
};

void print_houses()
{
    for (int i = 0; i < LIST_MAX; i++)
    {
        printf("{{%d,%d},{%d,%d}},\n", hList[i].p1.x, hList[i].p1.y,hList[i].p2.x, hList[i].p2.y);
    }
    printf("--------------------\n");
}
int main()
{
    print_houses();

    int curr = 0, next = 1; // indexes
    Point_t pList[LIST_MAX * 4];
    memset(pList, 0, sizeof(pList));

    int pIdx = 0; // current index in pList

    Point_t pt = {hList[curr].p1.x, hList[curr].p2.y};
    pList[pIdx++] = pt;

    pt.y = hList[curr].p1.y; // upper left of the most left house
    pList[pIdx++] = pt;

    for (;next < LIST_MAX; curr++, next++)
    {
        if (hList[curr].p2.x > hList[next].p1.x)
        {
            pt.x = hList[next].p1.x;
            pt.y = hList[curr].p1.y;
            pList[pIdx++] = pt;
            pt.y = hList[next].p1.y;
        }
        else
        {
            pt.x = hList[curr].p2.x;
            pt.y = hList[curr].p1.y;
            pList[pIdx++] = pt;

            pt.y = hList[curr].p2.y;
            pList[pIdx++] = pt;

            pt.x = hList[next].p1.x;
            pt.y = hList[curr].p2.y;
            pList[pIdx++] = pt;

            pt.x = hList[next].p1.x;
            pt.y = hList[next].p1.y;
            pList[pIdx++] = pt;
        }
    }

    pt.x = hList[curr].p2.x;
    pt.y = hList[curr].p1.y;
    pList[pIdx++] = pt;

    pt.y = hList[curr].p2.y;
    pList[pIdx++] = pt;

    // draw list
    for (int i = 0; i < pIdx; i++)
    {
        printf("(%d,%d)\n", pList[i].x, pList[i].y);
    }
    return 0;
}

#elif 1
#define MAX_LEN 15
int main()
{
    char filter = '5';
    printf("main 1 started, filtering for %c\n", filter);

    char input[MAX_LEN+1];
    char output[MAX_LEN+1];

    memset(output, 0, sizeof(output));

    fgets(input, sizeof(input), stdin);

    printf("input : %s\n", input);
    printf("output: ");

    for(int i = 0; (i < MAX_LEN) && (i < (int) strlen(input)); i++)
    {
        if (input[i] > '9' || input[i] < '0')
        {
            break;
        }

        output[i]='0';

        if (input[i] == filter)
        {
            printf("%c", input[i]-1);
            output[i]='1';
        }
        else
        {
            printf("%c", input[i]);
        }
    }

    char *pStart = output;
    for (int i = 0; i < (int) strlen(output); i++)
    {
        if (*pStart == '0')
            pStart++;
        else
            break;
    }

    if (strlen(pStart) > 0)
        printf(" + %s\n", pStart);
    else
        printf("\n");

    return 0;
}

#elif 0
#define SAVINGS_MAX 10000
#define SHARES_MAX  4
char* names[SHARES_MAX] = {"AAPL", "AMZN", "GOOG", "NFLX"};
int current[SHARES_MAX] = {148, 3293, 2856, 547};
int future[SHARES_MAX] = {165, 4152, 3016, 611};
int numbers[SHARES_MAX] = {1,1,1,1};

typedef struct
{
    int count;
    int index[SHARES_MAX];
    int profit;
    int invest;
    int sidx;
} result_t;

void calculate(const int shares_count, int savings)
{
    int total_invest = 0;
    int total_profit = 0;
    float percentage[shares_count];
    int proflist[shares_count];
    int price_max = 0;
    result_t rlist[shares_count * shares_count];
    memset(rlist, 0, sizeof(rlist));

    for (int i = 0; i < shares_count; i++)
    {
        int profit = future[i] - current[i];
        proflist[i] = profit > 0 ? profit : 0;
        percentage[i] = profit > 0 ? ((float) (profit * 100) / current[i]) : 0;
        total_invest += profit > 0 ? current[i] : 0;
        total_profit += profit > 0 ? profit : 0;

        if (price_max < current[i])
            price_max = current[i];

        printf("%d. %s curr[%d] -> future[%d], profit = %4d, income %5.2f%%\n", i+1,
               names[i], current[i], future[i], proflist[i], percentage[i]);
    }
    printf("\n");

    for (int numpass = 0; numpass < 2; numpass++)
    {
        if (numpass)
        {
            // let's adjust prices to medium-rare
            printf("\n------------------------\n");
            printf("recommended shares number:\n");
            for (int i = 0; i < shares_count; i++)
            {
                int nums = price_max / current[i];
                if (!nums) nums = 1;
                printf("%d. %s curr[$%d] recommended %d shares for total $%d\n", i+1,
                       names[i], current[i], nums, nums*current[i]);
                numbers[i] = nums;
                current[i] *= nums;
                future[i] *= nums;
            }
            printf("------------------------\n");

            total_invest = 0;
            total_profit = 0;
            memset(rlist, 0, sizeof(rlist));

            for (int i = 0; i < shares_count; i++)
            {
                int profit = future[i] - current[i];
                proflist[i] = profit > 0 ? profit : 0;
                percentage[i] = profit > 0 ? (float) (profit * 100 / current[i]) : 0;
                total_invest += profit > 0 ? current[i] : 0;
                total_profit += profit > 0 ? profit : 0;
            }
        }

        if (total_invest <= savings)
        {
            printf("total_invest[%d] <= savings[%d], can buy all shares\n", total_invest, savings);
            printf("-----------------------\n");
            int print_count = 1;
            for (int i = 0; i < shares_count; i++)
            {
                if (percentage[i] > 0)
                {
                    printf("%d. %s $%d \n", print_count++, names[i], current[i]);
                }
            }
            printf("total_profit = %d\n", total_profit);
        }
        else
        {
            int ridx = 0;
            for (int i = 0; i < shares_count; i++, ridx++)
            {
                if (!proflist[i])
                    continue;
                if (ridx >= shares_count * shares_count)
                {
                    printf("\n---------------\n ridx too big, abort\n");
                    exit(1);
                }

                rlist[ridx].profit += proflist[i];
                rlist[ridx].invest = current[i];
                rlist[ridx].index[rlist[ridx].count++] = i;


                for (int sidx = i + 1; sidx < shares_count; sidx++)
                {
                    for (int j = sidx; j < shares_count; j++)
                    {
                        if (!proflist[j])
                            continue;

                        if (rlist[ridx].invest + current[j] <= savings)
                        {
                            rlist[ridx].invest += current[j];
                            rlist[ridx].profit += proflist[j];
                            rlist[ridx].index[rlist[ridx].count++] = j;
                        }
                    }

                    ridx++;
                    rlist[ridx].profit += proflist[i];
                    rlist[ridx].invest = current[i];
                    rlist[ridx].index[rlist[ridx].count++] = i;
                }
            }

            // print out results and check the best profit
            int final_ridx = 0;
            total_profit = 0;
            for (int i = 0; i < ridx; i++)
            {
                printf("%2d. ", i+1);
                for (int j = 0; j < rlist[i].count; j++)
                {
                    printf("%s [%d x $%d] ", names[rlist[i].index[j]], numbers[rlist[i].index[j]], current[rlist[i].index[j]]);
                }
                printf("invest $%d, profit $%d\n", rlist[i].invest, rlist[i].profit);

                if (total_profit < rlist[i].profit)
                {
                    total_profit = rlist[i].profit;
                    final_ridx = i;
                }
            }

            printf("--------------------\n");
            printf("best case idx %d\n", final_ridx + 1);
            for (int j = 0; j < rlist[final_ridx].count; j++)
            {
                printf("%s [%d x $%d] ", names[rlist[final_ridx].index[j]], numbers[rlist[final_ridx].index[j]], current[rlist[final_ridx].index[j]]);
            }

            printf(" = invest $%d, profit $%d\n", rlist[final_ridx].invest, rlist[final_ridx].profit);
        }
    }
    printf("-----------------------\nall done.\n");
}

int main()
{
    printf("main 1 started...\n");

    calculate(SHARES_MAX, SAVINGS_MAX);
    return 0;
}

#elif 0

char input[] = "1*2*1 +10 +3*2*3 +5*0+ 2*2"; // 34

char* get_int(char* in, int* int_out)
{
    *int_out = 0;
    for (; in && *in && *in != '*' && *in != '+'; in++)
    {
        *int_out *= 10;
        *int_out += *in - '0';
    }
    return in;
}

int main()
{
    printf("main started...\n");

    // remove spaces
    int count = 0;
    for (int i = 0; i < (int) strlen(input); i++)
    {
        if (input[i] != ' ')
        {
            input[count++] = input[i];
        }
    }
    input[count]='\0';

    // main loop of additions
    int res =#ifdef MAIN_PREVIOUS
 0;
    int val = 0;

    for (char* tmp = input; *tmp; tmp += *tmp?1:0)
    {
        tmp = get_int(tmp, &val);

        if (*tmp != '*')
        {
            res += val;
        }
        else
        {
            int mltp_res = 1;

            do // sub-loop to collect all items to multiply
            {
                mltp_res *= val;
                tmp = get_int(++tmp, &val);
            } while (*tmp == '*');

            res += mltp_res * val;
        }
   }

   printf("%s = %d\n", input, res);
   return 0;
}
#endif
#endif // #ifdef MAIN_PREVIOUS
