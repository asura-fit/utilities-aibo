/****************************************/
/* thresh2tresh							*/
/*  input filename:	thresh2.tm2			*/
/*  output filename:	thresh.tml2		*/
/****************************************/

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>

#define	INFILE			("thresh2.tm2")
#define	OUTFILE			("thresh.tm2")
#define BUFFSIZE		(128)
#define HEADER			("TMAP")

// macro functions ---------------------------------------------------
#if (_DEBUG == 1)
	#define DBGMSG(x)	{ fprintf(stderr, x); }
#else
	#define DBGMSG(x)	{}
#endif // _DEBUG == 1


// global variables --------------------------------------------------
unsigned char remapTable[] = {
	0, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
};

// prototype declaration ----------------------------------------------
void assert(bool);
void skipLfCr(FILE *);

// fucntion implementation -------------------------------------------

void skipLfCr(FILE *fin){
	char b;
	size_t s;
	do {
		s = fread( &b, 1, 1, fin );
		assert(s != -1);
	} while( b == '\n' || b == '\r' );
	ungetc( b, fin );
}

void assert(bool b){
	static unsigned long count = 1000;
	count ++ ;
	if(!b){
		fprintf(stderr, "error occur.\n");
		exit(count);
	}
}

int main( int argc, char **argv )
{
	FILE *fp = NULL;
	FILE *fo = NULL;
	unsigned char buff[BUFFSIZE];
	size_t ret;

	fp = fopen( INFILE, "rb");
	if( fp == NULL ){
		fprintf(stderr, "cannot open file : %s\n", argv[1]);
		return 2;
	}
	fo = fopen( OUTFILE, "wb");
	if( fo == NULL ){
		fprintf(stderr, "cannot open output file\n");
		return 2;
	}

	// ヘッダ確認
	DBGMSG("checking header\n");
	ret = fread( buff, 1, 4, fp);
	if( ret != 4 || !(buff[0] == 'T' && buff[1] == 'M' && buff[2] == 'A' && buff[3] == 'P') ){
		fprintf(stderr, "header format error. %d \n", ret);
		return 3;
	}
	fprintf(fo, "TMAP\n");

	skipLfCr( fp );

	// コメント読み取り
	DBGMSG("reading comments\n");
	bool isLfCr = false;
	while( true ) {
//		std::string str;
		bool lc;
//		ret = fread( buff, 1, 1, fp );
		buff[0] = getc(fp);
//		assert( ret != -1);	// EOF対策
		assert( ret != EOF );
		lc = ( buff[0] == '\n' || buff[0] == '\r' );
		if( lc ) { // 改行コード
			if( !isLfCr ) {
				// 行が終わった
//				fprintf( fo, "%s\n", str.c_str() );
//				printf("%s\n", str.c_str() );
//				str = "";
			}
		}
		else { // 改行コードじゃない
			if( isLfCr &&  buff[0] != '#') {
				// # がこないならコメントは終わったんでしょう
				break;
			}
			// 改行コードでないなら書き込み
//			buff[1] = '\0';
//			str += string(buff);
		}

		fprintf( fo, "%c", buff[0] );
//		fprintf( stdout, "%c", buff[0] );
		isLfCr = lc;
	}

	// データヘッダ読み取り
	DBGMSG("reading data header\n");
	ret = fread( &buff[1], 1, 3, fp);
	assert(ret == 3);
	if( !(buff[0] == 'Y' && buff[1] == 'U' && buff[2] == 'V' && buff[3] == '8') ){
		fprintf(stderr, "data header error.\n");
		return 4;
	}
	fprintf( fo, "YUV8\n" );

	int x,y,z;
	fscanf(fp, "%d %d %d", &x, &y, &z);
//	printf("size = %d %d %d\n", x, y, z);
	fprintf(fo, "%d %d %d\n", x, y, z);
/*
	do {
		ret = fread( buff, 1, 1, fp );
		assert(ret != -1);
	} while(buff[0] == '\n' || buff[0] == '\r');
	*/
	skipLfCr(fp);

	DBGMSG("reading data\n");
	for( int i = 0; i < x * y * z; i++ ){
		ret = fread( buff, 1, 1, fp );
		assert(ret != -1);
//		if( ret != -1 )	break;
		if( buff[0] != 0xFF )
			fwrite( &remapTable[ buff[0] ], 1, 1, fo );
		else
			fwrite( &buff[0], 1, 1, fo );
	}

	fclose(fp);
	fclose(fo);

	DBGMSG("fin!\n");
}

