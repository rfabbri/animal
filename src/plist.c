#include <animal/animal.h>

main()
{
   list *L;
   int a;
   int data[]={11,3,69,0,0,0,0,0,0,0};
   SIQueue *q;

   animal_err_set_trace( ANIMAL_ERR_TRACE_ON);

   /*
    *  Test integer linked list
    */
   L = new_list();

   list_prepend(&L, 2);
   list_append(&L, 269);
   list_append(&L, 73);
   list_append(&L, 49);
   free_list(&L);

   /*
    *  Test Static Integer Queue
    */
   q = siq_new_from_static_array(data,10,3);
   siq_remove(q, &a);
   siq_remove(q, &a);
   siq_insert(q, 222);
   siq_remove(q, &a);
   printf("removed: %d\n", a);
   siq_print(q);
   siq_free(&q);

   animal_heap_report(NULL);
   return 0;
}
