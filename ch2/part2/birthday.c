#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>  /* struct list_head */


/// The structure of Birthday element.
typedef struct {
  int day;
  int month;
  int year;
  struct list_head list;
} Birthday;

/**
 * @brief Allocates a Birthday element with its date content initialized.
 * @param day, month, year The day, month, year of the initialized element.
 * @return Birthday*: A pointer to the newly allocated Birthday element.
 */
Birthday* AllocateNewBirthday(const int day, const int month, const int year) {
  Birthday* birthday = kmalloc(sizeof(*birthday), GFP_KERNEL /* may sleep and swap to free memory */);
  birthday->day = day;
  birthday->month = month;
  birthday->year = year;
  /* initialize the "list" member */
  INIT_LIST_HEAD(&birthday->list);
  return birthday;
}

/**
 * @brief Prints the date contents of Birthday elements to kernel log buffer in format DD/MM/YYYY.
 * @param head The head of the list which links the elements to be print.
 */
void PrintBirthdaysToLogBuffer(struct list_head* head) {
  int i = 1;
  Birthday* curr_birthday = NULL;
  list_for_each_entry(curr_birthday /* points to the entry in each iteration */,
                      head,
                      list /* name of the list_head member in Birthday */) {
    printk(KERN_INFO "Birthday %d has date %02d/%02d/%d.\n",
           i++, curr_birthday->day, curr_birthday->month, curr_birthday->year);
  }
}

/**
 * @brief Frees (deallocate) all Birthday elements linked in the list.
 * @param head The head of the list which links the elements to be free.
 */
void FreeBirthdayList(struct list_head* head) {
  int i = 1;
  /* "next" is an extra pointer to maintain the list under entry modifications */
  Birthday* curr = NULL, * next = NULL;
  list_for_each_entry_safe(curr, next, head, list) {
    list_del(&curr->list);
    kfree(curr);
    printk(KERN_INFO "Birthday %d is deleted!\n", i++);
  }
}

/* declare a list head object */
LIST_HEAD(birthday_list);

/**
 * @brief Module entry point: creates a birthday list with 5 Birthday elements
 * and traverses them to output their date contents to kernel log buffer.
 */
int InitBirthdayList(void) {
  Birthday* birthdays[] = {
    AllocateNewBirthday(2, 8, 1995),
    AllocateNewBirthday(12, 15, 2000),
    AllocateNewBirthday(3, 21, 1999),
    AllocateNewBirthday(9, 3, 1985),
    AllocateNewBirthday(7, 29, 1991),
  };
  int i = 0;
  printk(KERN_INFO "Creating birthday list...\n");
  for (; i < sizeof(birthdays) / sizeof(Birthday*); ++i) {
    /* add into the list structure */
    list_add_tail(&birthdays[i]->list, &birthday_list);
  }

  PrintBirthdaysToLogBuffer(&birthday_list);

  printk(KERN_INFO "Creation ends.\n");
  return 0;
}

/**
 * @brief Module exit point: deletes all Birthday elements previously created in
 * the entry point..
 */
void ExitBirthdayList(void) {
  printk(KERN_INFO "Deleting birthday list...\n");

  FreeBirthdayList(&birthday_list);

  printk(KERN_INFO "Deletion ends.\n");
}


module_init(InitBirthdayList);
module_exit(ExitBirthdayList);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Perform kernel data structure (circular, doubly linked list) operations.");
MODULE_AUTHOR("Lai-YT");
