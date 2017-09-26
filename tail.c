#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define DEAFULT 10
#define ILLEGAL_OPTION -99
#define ERROR_CODE -999

char buf[512];

int linesNumberToPrint(char* argvs[]) {
	char** argv = argvs;
	int i = 1;
	char number[100];

	if(argv[1][0] == '-') {
		for(i=1; i<strlen(argv[1]);i++) {
			if(((int)(argv[1][i]))>=48 && ((int)(argv[1][i]))<=57) {
				number[i-1] = argv[1][i];
			}
			else {
				return ILLEGAL_OPTION; // Illegal Option
			}
		}
		int linesToDisplay = (atoi)(number);
		return linesToDisplay;
	}
	else {
		for(i=0; i<strlen(argv[1]);i++) {
			if(((int)(argv[1][i]))>=48 && ((int)(argv[1][i]))<=57) {
				number[i] = argv[1][i];
			}
			else {
				return ERROR_CODE;
			}
		}
		return ERROR_CODE;
	}
}

void tail(int fd, int linesToPrint)
{
	int i, n;
	int linesInFile = 0;
	int linesReadInTempFile = 0;
	int linesToPrintFn = linesToPrint;
	int lineIndexPrintStart;

	int tempFile = open("tempTailFile",O_CREATE | O_RDWR);

  // Writing in temporaryFile
	while((n = read(fd, buf, sizeof(buf))) > 0){
		write(tempFile, buf, n);
		for(i=0; i<n; i++){
			if(buf[i] == '\n')
				linesInFile++;
		}
	}
	close(tempFile);

	lineIndexPrintStart = linesInFile - linesToPrintFn;

	//Open the file again to read the lines
	tempFile = open("tempTailFile", O_RDONLY);
	while((n = read(tempFile, buf, sizeof(buf))) > 0) {
		for(i=0; i<n; i++) {
			if(linesReadInTempFile >= lineIndexPrintStart) {
				printf(1, "%c", buf[i]);
			}
			if(buf[i] == '\n') {
				linesReadInTempFile++;
			}
		}
	}
	// To delete the temporary file
	unlink("tempTailFile");
}

int main(int argc, char *argv[]) {
	int fd, numberOfLines;
	//If user is entering input from the command line
	if(argc <= 1) {
		tail(0,DEAFULT);
		exit();
	}

	else if(argc == 2) {

		numberOfLines = linesNumberToPrint(argv);
		fd = open(argv[1], O_RDONLY);

		// for handling case like
		//tail README
		if(fd >= 0) {
			tail(fd, DEAFULT);
			close(fd);
			exit();
		}
		
		// When file can't be opened and the paramater after
		// tail starts with '-' and it's an invalid number
		// eg cat README | tail -68ty
		else if ((fd < 0) && (numberOfLines == ILLEGAL_OPTION)) {
			printf(1, "Illegal Option\n");
			exit();
		}

		// When file can't be opened and the paramater
		// eg cat README | tail 68
		else if(fd < 0 && numberOfLines == ERROR_CODE) {
			printf(1, "tail: cannot open %s\n", argv[1]);
			exit();
		}

		// For handling cases like cat README | tail -9
		else if((fd < 0) && ((numberOfLines%2 == 0) || (numberOfLines%2 == 1))) {
			tail (0,numberOfLines);
			exit();
		}
		// When tail can't open a file
		else if (fd < 0) {
			printf(1, "tail: cannot open %s\n", argv[1]);
		}
	}

	else if(argc == 3) {

		numberOfLines = linesNumberToPrint(argv);

		if((fd = open(argv[2], O_RDONLY)) < 0){
			printf(1, "tail: cannot open %s\n", argv[2]);
			exit();
		}
		// If the number is -99 which means the paramater
		// after tail is not correct so we will output
		// Illegal Option error
		if(numberOfLines == ILLEGAL_OPTION) {
			printf(1, "Illegal option\n");
			exit();
		}
		numberOfLines = ((numberOfLines == ERROR_CODE) ? DEAFULT : numberOfLines);
			tail(fd, numberOfLines);
			close(fd);
			exit();		
	} 
	else {
		printf(1, "tail: cannot open %s\n", argv[2]);
		exit();
	}
	exit();
}
