//ライフゲームプログラム
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

//マスの更新ルール
#define DEPOPULATION 1
#define OVERCROWING 4
#define BIRTH 3

#define MAX_SIZE 50
#define MAX_STEP 100

//マスの状態を表す列挙体
enum Status
{
  DEAD,
  LIVING
};

//マップの構造体
typedef struct
{
  int size; //マップのサイズ
  enum Status mapData[MAX_SIZE][MAX_SIZE]; //生死のマップ(Status列挙体)
}Map;

//(x,y)の次の状態を返す関数
enum Status getNextStatus(Map *map, int x, int y)
{
  enum Status now = map->mapData[x][y];
  int size = map->size;
  int count = 0;
  int i, j;
  for(i = -1; i <= 1; i++){
    for(j = -1; j <= 1; j++){
      if((x + i) >= 0 && (x + i) < size && (y + j) >= 0 && (y + j) < size){
	if(i != 0 || j != 0){
	  if(map->mapData[x + i][y + j] == LIVING){
	    count++;
	  }
	}
      }
    }
  }
  if(now == LIVING){
    if(count <= DEPOPULATION || count >= OVERCROWING){
      return DEAD;
    }else{
      return LIVING;
    }
  }else{
    if(count == BIRTH){
      return LIVING;
    }else{
      return DEAD;
    }
  }
  
}

//beforeを次の状態に更新してafterに書き込む関数
void updateMap(Map *before, Map *after)
{
  int size = before->size;
  int i, j;
  for(i = 0; i < size; i++){
    for(j = 0; j < size; j++){
      after->mapData[i][j] = getNextStatus(before, i, j);
    }
  }
}

//マップの生死状態をランダムにセットする関数
void setRandomStatus(Map *map)
{
  int size = map->size;
  int i, j;
  srand((unsigned int)time(NULL));
  for(i = 0; i < size; i++){
    for(j = 0; j < size; j++){
      int r = rand();
      r = r % 8;
      if(r == 0){
	map->mapData[i][j] = LIVING;
      }else{
	map->mapData[i][j] = DEAD;
      }
    }
  }
}

//マップの状態を画面に表示する関数
void printMapData(Map map)
{
  int size = map.size;
  int i, j;
  for(i = 0; i < size; i++){
    for(j = 0; j < size; j++){
      if(map.mapData[i][j] == LIVING){
	printf("＠");
      }else{
	printf("・");
      }
    }
    printf("\n");
  }
}

//LIVINGのマスの数を返す関数
int countLiving(Map map)
{
  int count = 0;
  int size = map.size;
  int i, j;
  for(i = 0; i < size; i++){
    for(j = 0; j < size; j++){
      if(map.mapData[i][j] == LIVING){
	count++;
      }
    }
  }
  return count;
}

//Map構造体fromの中身をtoコピーする関数
void copyMap(Map *from, Map *to)
{
  int size = from->size;
  to->size = from->size;
  int i, j;
  for(i = 0; i < size; i++){
    for(j = 0; j < size; j++){
      to->mapData[i][j] = from->mapData[i][j];
    }
  }
}

//マップデータをファイル出力する関数
//成功したときは0、途中で停止したときは0を返す
int writeMapData(Map *map)
{
  FILE *fp;
  char filename[20];
  printf("ファイル名:");
  scanf("%s", &filename);

  //必要ならファイル名の末尾に".txt"を追加
  char str[] = ".txt";
  if(strstr(filename, str) == NULL){
    strcat(filename, str);
  }
  
  fp = fopen(filename, "w");
  if(fp == NULL){
  printf("ファイルが開けませんでした。\n");
  return 1;
}
  int mapSize = map->size;
  int x, y;
  char ch;
  for(x = 0; x < mapSize; x++){
  for(y = 0; y < mapSize; y++){
  if(map->mapData[x][y] == LIVING){
  ch = '@';
}else{
  ch = '.';
}      
  if(fputc(ch, fp) == EOF){
  printf("ファイルの書き込みに失敗しました。\n");
  return 1;
}
}
  if(fputc('\n', fp) == EOF){
  printf("ファイルの書き込みに失敗しました。\n");
  return 1;
}
}
  fclose(fp);
  printf("マップを%sに出力しました。\n", filename);
  return 0;
}

//マップデータをファイル出力する関数（バイナリ版）
//成功したときは0、途中で停止したときは0を返す
int writeMapDataB(Map *map)
{
  FILE *fp;
  char filename[20];
  printf("ファイル名:");
  scanf("%s", &filename);
  fp = fopen(filename, "wb");
  if(fp == NULL){
    printf("ファイルが開けませんでした。\n");
    return 1;
  }
  int mapSize = map->size;
  if(fwrite(&mapSize, sizeof(int), 1, fp) < 1){
    printf("ファイルの書き込みに失敗しました。\n");
    return 1;
  }
  int x, y;
  char ch;
  for(x = 0; x < mapSize; x++){
    for(y = 0; y < mapSize; y++){
      if(map->mapData[x][y] == LIVING){
	ch = '1';
      }else{
	ch = '0';
      }
      if(fwrite(&ch, sizeof(char), 1, fp) < 1){
	printf("ファイルの書き込みに失敗しました。\n");
	return 1;
      }
    }
  }
  fclose(fp);
  printf("マップを%sに出力しました。\n", filename);
  return 0;
}

//ファイルからマップを読み込む関数
//成功したときは0、途中で停止したときは1を返す
int readMapData(Map *map)
{
  FILE *fp;
  char filename[20];
  printf("ファイル名:");
  scanf("%s", &filename);
  
  //必要ならファイル名の末尾に".txt"を追加
  char str[] = ".txt";
  if(strstr(filename, str) == NULL){
    strcat(filename, str);
  }
  
  fp = fopen(filename, "r");
  if(fp == NULL){
    printf("ファイルが開けませんでした。\n");
    return 1;
  }
  int mapSize;
  char ch;
  int x = 0;
  int y = 0;
  while((ch = fgetc(fp)) != EOF){
    if(ch != '\n'){
      if(ch == '.'){
	map->mapData[x][y] = DEAD;
	y++;
      }else if(ch == '@'){
	map->mapData[x][y] = LIVING;
	y++;
      }else{
	printf("ファイルの形式が不正です。\n");
	return 1;
      }      
    }else{
      //列と行の数が一致しているかチェック
      if(x == 0){
	mapSize = y;
      }else{
	if(mapSize != y){
	  printf("ファイルの形式が不正です。\n");
	  return 1;
	}
      }
      x++;
      y = 0;
    }
  }
  fclose(fp);
  map->size = mapSize;
  return 0;
}

//ファイルからマップを読み込む関数(バイナリ版)
//成功したときは0、途中で停止したときは1を返す
int readMapDataB(Map *map)
{
  FILE *fp;
  char filename[20];
  printf("ファイル名:");
  scanf("%s", &filename);
  fp = fopen(filename, "rb");
  if(fp == NULL){
    printf("ファイルが開けませんでした。\n");
    return 1;
  }
  int mapSize;
  if(fread(&mapSize, sizeof(int), 1, fp) < 1){
    printf("ファイルの読み込みに失敗しました。\n");
    return 1;
  }
  int x, y;
  char ch;
  for(x = 0; x < mapSize; x++){
    for(y = 0; y < mapSize; y++){
      if(fread(&ch, sizeof(char), 1, fp) < 1){
	printf("ファイルの読み込みに失敗しました。\n");
	return 1;
      }
      if(ch == '0'){
	map->mapData[x][y] = DEAD;
      }else if(ch == '1'){
	map->mapData[x][y] = LIVING;
      }else{
	printf("ファイルの形式が不正です。\n");
	return 1;
      }
    }
  }
  fclose(fp);
  map->size = mapSize;
  return 0;
}

//メイン関数
int main()
{
  int menu = -1;
  printf("初期配置をファイルから読み込みますか?\n");
  while(menu < 0 || menu > 2){
    printf("テキストファイルから読み込む・・・0,バイナリデータから読み込む・・・1,ランダムに配置・・・2:");
    scanf("%d", &menu);
    //無限ループ回避
    if(menu < 0 || menu > 2){
      scanf("%*s");
    }
  }

  Map map1, map2;

  //テキストからマップを読み込む
  if(menu == 0){
    if(readMapData(&map1)){
      return 1;
    }else{
      printMapData(map1);
      printf("ファイルからマップを読み込みました。\n");
    }
  }

  //バイナリデータからマップを読み込む
  else if(menu == 1){
    if(readMapDataB(&map1)){
      return 1;
    }else{
      printMapData(map1);
      printf("ファイルからマップを読み込みました。\n");
    }
  }
  
  //ランダムに初期配置
  else if(menu == 2){
    int mapSize = -1;
    while(mapSize < 0 || mapSize > MAX_SIZE){
      printf("マップサイズを決めてください(3～%d):", MAX_SIZE);
      scanf("%d", &mapSize);
      //無限ループ回避
      if(mapSize < 0 || mapSize > MAX_SIZE){
        scanf("%*s");
      }
    }
    map1.size = mapSize;
    setRandomStatus(&map1);
    printMapData(map1);
  }

  copyMap(&map1, &map2);
  
  //メインループ
  while(1){
    int step = -1;
    while(step < 0 || step > MAX_STEP){
      printf("何ステップ進めますか?(0を入力すると終了します):");
      scanf("%d", &step);
      //無限ループ回避
      if(step < 0 || step > MAX_STEP){
	scanf("%*s");
      }
    }
    if(step == 0){
      printf("現在の状態をファイルに保存しますか?");
      menu = -1;
      while(menu < 0 || menu > 2){
	printf("テキストファイルで保存・・・0、バイナリデータで保存・・・1、保存しない・・・2:");
	//無限ループ回避
	scanf("%d", &menu);
	if(menu < 0 || menu > 2){
	  scanf("%*s");
	}
      }

      if(menu == 2){
	return 0;
      }
      
      //関数ポインタの宣言
      int (*writeFunc)(Map *map);

      if(menu == 0){
	//テキストファイルに出力
	writeFunc = writeMapData;
      }else if(menu == 1){
	//バイナリデータに出力
	writeFunc = writeMapDataB;
      }
      writeFunc(&map1);
      return 0;
    }
    
    int i;
    for(i = 0; i < step; i++){
      updateMap(&map1, &map2);
      copyMap(&map2, &map1);
      system("clear");
      printMapData(map1);
      if(countLiving(map1) == 0){
	printf("絶滅しました。\n");
	return 0;
      }
      sleep(1);
    }
  }
}
