#include <linux/init.h>     // Інклюд для функцій ініціалізації та очищення модуля
#include <linux/ktime.h>    // Інклюд для роботи з часом у ядрі
#include <linux/list.h>     // Інклюд для роботи зі списками ядра
#include <linux/module.h>   // Основний інклюд для роботи з модулями
#include <linux/printk.h>   // Інклюд для функцій логування
#include <linux/slab.h>     // Інклюд для функцій управління пам'яттю

// Метадані модуля
MODULE_AUTHOR("Yaroslav Latypov <jarlat04@lapuas.xyz>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel");
MODULE_LICENSE("Dual BSD/GPL");

// Параметр модуля: кількість разів для друку "Hello, world!"
static unsigned int print_count = 1;
module_param(print_count, uint, 0444); // Оголошення параметра модуля
MODULE_PARM_DESC(print_count, "A parameter that determines the number of times 'Hello, world!' is printed");

// Структура для зберігання часу кожного виведення
struct hello_time {
  struct list_head list; // Список для зв'язування структур
  ktime_t time;          // Часова мітка для запису
};

// Статична змінна для голови списку
static LIST_HEAD(hello_time_list);

// Функція ініціалізації модуля
static int __init hello_init(void) {
  struct hello_time *time_entry;
  unsigned int i;

  // Перевірка та обробка значення параметра print_count
  if (print_count == 0 || (print_count >= 5 && print_count <= 10)) {
    pr_warn("Warning: print_count is 0 or between 5 and 10\n");
  } else if (print_count > 10) {
    pr_err("Error: print_count is greater than 10\n");
    return -EINVAL;
  }

  // Цикл для друку повідомлення і зберігання часу
  for (i = 0; i < print_count; i++) {
    time_entry = kmalloc(sizeof(*time_entry), GFP_KERNEL); // Виділення пам'яті
    time_entry->time = ktime_get();                        // Запис часу
    list_add_tail(&time_entry->list, &hello_time_list);    // Додавання елемента в список
    pr_emerg("Hello, world!\n");                           // Друк повідомлення
  }

  return 0;
}

// Функція очищення модуля
static void __exit hello_exit(void) {
  struct hello_time *time_entry, *tmp;

  // Прохід по списку для виведення часу та очищення пам'яті
  list_for_each_entry_safe(time_entry, tmp, &hello_time_list, list) {
    pr_info("Time: %lld\n", ktime_to_ns(time_entry->time)); // Виведення часу
    list_del(&time_entry->list);                            // Видалення зі списку
    kfree(time_entry);                                      // Звільнення пам'яті
  }
}

// Реєстрація функцій ініціалізації та очищення
module_init(hello_init);
module_exit(hello_exit);
