#include <stdio.h>//标准输入输出头文件，如printf、scanf
#include <stdlib.h>//标准库头文件，如malloc、free
#include <string.h>//字符串处理头文件，如strcpy、strlen
#include <ctype.h>//字符处理头文件，如判断字符是否为字母数字
#include <stdbool.h>//提供bool类型
#include <limits.h>//定义各种数据类型的取值范围，如INT_MAX

#define FILENAME "employee_data.dat"//数据文件存储名
#define MAX_NAME_LEN 50//名字最大长度
#define MAX_EMPLOYEES 1000//最大员工限制
//链表存储数据
typedef struct Employee {
    char name[MAX_NAME_LEN];//名字
    int id;//工号
    float basic_salary, job_salary, allowance, insurance, fund, total_salary;//基本工资，薪水，津贴，保险，基金，总工资
    struct Employee *next;//链表指针
} Employee;

// 函数声明
void displayMenu();
bool addEmployee(Employee **head, int *count);
void displayAll(Employee *head);
bool modifyEmployee(Employee *head);
bool searchEmployee(Employee *head);
bool deleteEmployee(Employee **head, int *count);
void statistics(Employee *head, int count);
bool saveToFile(Employee *head);
bool loadFromFile(Employee **head, int *count);
void freeList(Employee **head);
void calculateTotal(Employee *emp);
Employee* findEmployeeById(Employee *head, int id);
void printAllAsTable(Employee *head);
void pressAnyKey();

int main() {
    Employee *head = NULL;//链表头指针
    int choice, count = 0;//当前员工计数
    
    if (!loadFromFile(&head, &count)) 
        printf("警告: 数据加载失败或不存在\n");
    else
        printf("成功加载 %d 条记录\n", count);
    pressAnyKey();//启动时加载数据
    
    do {
        system("cls||clear");
        displayMenu();
        printf("请选择(1-8): ");
        scanf("%d", &choice);
        while(getchar() != '\n');
        //显示菜单并处理用户选择
        switch(choice) {
            case 1: if(addEmployee(&head, &count)) printf("添加成功!\n"); break;
            case 2: displayAll(head); break;
            case 3: if(modifyEmployee(head)) printf("修改成功!\n"); break;
            case 4: searchEmployee(head); break;
            case 5: if(deleteEmployee(&head, &count)) printf("删除成功!\n"); break;
            case 6: statistics(head, count); break;
            case 7: if(saveToFile(head)) printf("保存成功!\n"); break;
            case 8: printf("退出系统...\n"); break;
            default: printf("无效选择!\n");
        }
        if(choice != 8) pressAnyKey();
    } while(choice != 8);
    
    freeList(&head);//退出前释放内存
    return 0;
}

void displayMenu() {
    printf("\n=== 职工工资管理系统 ===\n");
    printf("1. 添加职工记录\n2. 显示所有记录\n3. 修改记录\n");
    printf("4. 查找记录\n5. 删除记录\n6. 统计功能\n");
    printf("7. 保存数据\n8. 退出系统\n=======================\n");
}

bool addEmployee(Employee **head, int *count) {
    if (*count >= MAX_EMPLOYEES) {
        printf("已达最大职工数!\n");
        return false;
    }
    
    Employee *newEmp = (Employee*)malloc(sizeof(Employee));
    if (!newEmp) return false;
    
    printf("姓名: "); 
    fgets(newEmp->name, MAX_NAME_LEN, stdin);
    newEmp->name[strcspn(newEmp->name, "\n")] = '\0';
    
    printf("ID: "); 
    scanf("%d", &newEmp->id);
    if(findEmployeeById(*head, newEmp->id)) {
        printf("ID已存在!\n");
        free(newEmp);
        return false;
    }
    
    printf("基本工资 职务工资 岗位津贴: ");
    scanf("%f %f %f", &newEmp->basic_salary, &newEmp->job_salary, &newEmp->allowance);
    printf("医疗保险 公积金: ");
    scanf("%f %f", &newEmp->insurance, &newEmp->fund);
    while(getchar() != '\n');
    
    calculateTotal(newEmp);
    newEmp->next = *head;
    *head = newEmp;
    (*count)++;
    return true;
}

void displayAll(Employee *head) {
    if (!head) {
        printf("没有记录!\n");
        return;
    }
    printAllAsTable(head);
}

bool modifyEmployee(Employee *head) {
    if (!head) {
        printf("没有记录!\n");
        return false;
    }
    
    int id;
    printf("输入要修改的ID: ");
    scanf("%d", &id);
    while(getchar() != '\n');
    
    Employee *emp = findEmployeeById(head, id);
    if (!emp) {
        printf("未找到ID %d\n", id);
        return false;
    }
    
    printf("新姓名(%s): ", emp->name);
    fgets(emp->name, MAX_NAME_LEN, stdin);
    emp->name[strcspn(emp->name, "\n")] = '\0';
    
    printf("新基本工资 职务工资 岗位津贴: ");
    scanf("%f %f %f", &emp->basic_salary, &emp->job_salary, &emp->allowance);
    printf("新医疗保险 公积金: ");
    scanf("%f %f", &emp->insurance, &emp->fund);
    while(getchar() != '\n');
    
    calculateTotal(emp);
    return true;
}

bool searchEmployee(Employee *head) {
    if (!head) {
        printf("没有记录!\n");
        return false;
    }
    
    int choice;
    printf("1.按ID查找 2.按姓名查找: ");
    scanf("%d", &choice);
    while(getchar() != '\n');
    
    if (choice == 1) {
        int id;
        printf("输入ID: ");
        scanf("%d", &id);
        while(getchar() != '\n');
        
        Employee *emp = findEmployeeById(head, id);
        if (emp) {
            printf("\n姓名: %s\nID: %d\n总工资: %.2f\n", 
                  emp->name, emp->id, emp->total_salary);
            return true;
        }
    } else {
        char name[MAX_NAME_LEN];
        printf("输入姓名: ");
        fgets(name, MAX_NAME_LEN, stdin);
        name[strcspn(name, "\n")] = '\0';
        
        Employee *current = head;
        while (current) {
            if (strcmp(current->name, name) == 0) {
                printf("\n姓名: %s\nID: %d\n总工资: %.2f\n", 
                      current->name, current->id, current->total_salary);
                return true;
            }
            current = current->next;
        }
    }
    printf("未找到记录!\n");
    return false;
}

bool deleteEmployee(Employee **head, int *count) {
    if (!*head) {
        printf("没有记录!\n");
        return false;
    }
    
    int id;
    printf("输入要删除的ID: ");
    scanf("%d", &id);
    while(getchar() != '\n');
    
    Employee *current = *head, *prev = NULL;
    while (current && current->id != id) {
        prev = current;
        current = current->next;
    }
    
    if (!current) {
        printf("未找到ID %d\n", id);
        return false;
    }
    
    printf("确认删除 %s?(y/n): ", current->name);
    if (getchar() == 'y') {
        if (prev) prev->next = current->next;
        else *head = current->next;
        free(current);
        (*count)--;
        return true;
    }
    while(getchar() != '\n');
    return false;
}

void statistics(Employee *head, int count) {
    if (!head) {
        printf("没有记录!\n");
        return;
    }
    
    float total = 0;
    Employee *current = head;
    while (current) {
        total += current->total_salary;
        current = current->next;
    }
    
    printf("\n职工总数: %d\n", count);

    printf("平均总工资: %.2f\n", total/count);
    printf("按任意键查看详细统计...\n职工工资信息如下：\n");
    getchar();
    printAllAsTable(head);
}

bool saveToFile(Employee *head) {
    FILE *file = fopen(FILENAME, "wb");
    if (!file) return false;
    
    Employee *current = head;
    while (current && fwrite(current, sizeof(Employee), 1, file) == 1)
        current = current->next;
    
    fclose(file);
    return !current;
}

bool loadFromFile(Employee **head, int *count) {
    FILE *file = fopen(FILENAME, "rb");
    if (!file) return false;
    
    Employee temp, *newEmp;
    *head = NULL;
    Employee *tail = NULL;
    
    while (fread(&temp, sizeof(Employee), 1, file) == 1 && *count < MAX_EMPLOYEES) {
        newEmp = (Employee*)malloc(sizeof(Employee));
        if (!newEmp) {
            fclose(file);
            freeList(head);
            return false;
        }
        *newEmp = temp;
        newEmp->next = NULL;
        
        if (!*head) *head = newEmp;
        else tail->next = newEmp;
        tail = newEmp;
        (*count)++;
    }
    
    fclose(file);
    return true;
}

void calculateTotal(Employee *emp) {
    emp->total_salary = emp->basic_salary + emp->job_salary + emp->allowance 
                      - emp->insurance - emp->fund;
}

Employee* findEmployeeById(Employee *head, int id) {
    while (head && head->id != id) head = head->next;
    return head;
}

void printAllAsTable(Employee *head) {
    printf("\n%-20s %-6s %-8s %-8s %-8s %-8s %-8s %-8s\n", 
          "姓名", "ID", "基本", "职务", "津贴", "保险", "公积金", "总计");
    printf("------------------------------------------------------------\n");
    while (head) {
        printf("%-20s %-6d %-8.2f %-8.2f %-8.2f %-8.2f %-8.2f %-8.2f\n",
              head->name, head->id, head->basic_salary, head->job_salary,
              head->allowance, head->insurance, head->fund, head->total_salary);
        head = head->next;
    }
}

void freeList(Employee **head) {
    Employee *tmp;
    while (*head) {
        tmp = *head;
        *head = (*head)->next;
        free(tmp);
    }
}

void pressAnyKey() {
    printf("\n按任意键继续...");
    while(getchar() != '\n');
}