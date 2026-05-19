#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int debug = 0;

double calc_expr_computing (double left_hand, char operator, double right_hand)
{
  switch(operator)
    {
    case '+':
      return left_hand + right_hand;
      break;
    case '-':
      return left_hand - right_hand;
      break;
    case '*':
      return left_hand * right_hand;
      break;
    case '/':
      return left_hand / right_hand;
      break;
    case '^':
      return pow (left_hand, right_hand);
      break;
    case '<':
      return ldexp (left_hand, (double) floor(right_hand));
      break;
    case '>':
      return ldexp (left_hand, ((double) floor(right_hand))*(-1));
      break;
    case '%':
      return fmod (left_hand, right_hand);
      break;
    default:
      if (debug > 0)
        {
          perror("error");
          return 0.0;
          break;
        }
    }
}

int main()
{
  double result,left_hand, right_hand;
  char operator;

  while ((scanf("%lf %c %lf", &left_hand, &operator, &right_hand)) == 3)
    {
      result = calc_expr_computing(left_hand, operator, right_hand);
      printf("%lf", result);
    }
}

