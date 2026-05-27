#include <stdio.h>
#include "first_c/first.h"
#include "first_c/first_types.h"

void main()
{
  int  x, y;

  /* Allocation des entrées */
  First__myfun_out o; /* Allocation des sorties */

  for (;;)
    {
      /* Boucle infinie */
      printf("Inputs:");
      scanf("%d%d", &x, &y); /* Lecture des entrées */
      First__myfun_step(x, y, &o);
      /* Calculs */
      printf("Result: z=%dt=%d\n", o.z, o.t);
      /* Ecriture des sorties */
    }
}
