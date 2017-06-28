typedef struct HashTable_s HashTable_t;

HashTable_t* TableCreate();
void TableDestroy();
void TableSet(HashTable_t* table, char* key, char* value);
char* TableGet(HashTable_t* table, char* key);
