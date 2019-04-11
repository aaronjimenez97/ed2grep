/*	Aaron Jimenez-Lopez
	 	Shorter and condensed version of the the program that the professor provided
 		Work to get rid of error handling and v flags?
 		create own read file and getch_() that calls the getchar() function
  	if there is nothing in the buffer
 */

#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <string.h>
/* make BLKSIZE and LBSIZE 512 for smaller machines */
#define	BLKSIZE	4096
#define	NBLK	2047
//#define	NULL	0
#define	FNSIZE	128
#define	LBSIZE	4096
#define	ESIZE	256
#define	GBSIZE	256
#define	NBRA	5
//#define	EOF	-1
#define	KSIZE	9
#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12
#define	CBACK	14
#define	CCIRC	15
#define	STAR	01
char	Q[]	= "";
char	T[]	= "TMP";
#define	READ	0
#define	WRITE	1
int	peekc;
int	lastc;
char	savedfile[FNSIZE];
char	file[FNSIZE];
char	linebuf[LBSIZE];
char	rhsbuf[LBSIZE/2];
char	expbuf[ESIZE+4];
int	given;
unsigned int	*addr1, *addr2;
unsigned int	*dot, *dol, *zero;
char	genbuf[LBSIZE];
long	count;
char	*nextip;
char	*linebp;
int	ninbuf;
int	io;
int	pflag;
long	lseek(int, long, int);
int	open(char *, int);
int	creat(char *, int);
int	read(int, char*, int);
int	write(int, char*, int);
int	close(int);
int	fork(void);
int	execl(char *, ...);
int	wait(int *);
int	unlink(char *);
int	vflag	= 1,	oflag,listf,listn,	col, tfile = -1, tline, ichanged, nleft, anymarks,subnewa, subolda;
char	*globp;
char	*tfname;
char	*loc1;
char	*loc2;
char	ibuff[BLKSIZE];
int	iblock	= -1;
char	obuff[BLKSIZE];
int	oblock	= -1;
char	WRERR[]	= "WRITE ERROR";
int	names[26];
int nbra, fchange,	wrapp,	bpagesize = 20;
unsigned nlall = 128;
char	*mktemp(char *);
char	tmpXXXXX[50] = "/tmp/eXXXXX";
char *getblock(unsigned int atl, int iof);
char *place(char *sp, char *l1, char *l2);
void add(int i);
int advance(char *lp, char *ep);
int append(int (*f)(void), unsigned int *a);
int backref(int i, char *lp);
int cclass(char *set, int c, int af);
void commands(void);
void compile(int eof);
int compsub(void);
void dosub(void);
void error(char *s);
int execute(unsigned int *addr);
void exfile(void);
void filename(int comm);
void gdelete(void);
int getchr(void);
int getcopy(void);
int getfile(void);
int getnum(void);
int getsub(void);
void global(int k);
void init(void);
unsigned int *address(void);
void join(void);
void move(int cflag);
void newline(void);
void nonzero(void);
//void onhup(int n);
//void onintr(int n);
void print(void);
void putchr(int ac);
void putd(void);
void putfile(void);
int putline(void);
//void puts(char *sp);
void quit(int n);
void rdelete(unsigned int *ad1, unsigned int *ad2);
void setwide(void);
//void setnoaddr(void);
void squeeze(int i);
typedef void	(*SIG_TYP)(int);
SIG_TYP	oldhup;
SIG_TYP	oldquit;
/* these two are not in ansi, but we need them */
#define	SIGHUP	1	/* hangup */
#define	SIGQUIT	3	/* quit (ASCII FS) */

char grepbuf[GBSIZE];
void greperror(char);  void grepline(void);
FILE *myfile;
void readfile_(const char*, const char *);
void ungetch_(int);
int getch_(void);
void readfile_(const char * open_file, const char * mode){
  const char* ofile = open_file;
  const char* file_options = mode;
  printf("Trying to open the file....\n" );
  myfile= fopen(ofile, file_options);
  if( myfile==NULL){
    printf("FILE NOT OPENED.");
  }
  else
  {//Store the files contents into a buffer
    setbuf(myfile, genbuf);
    printf("Set myfile into grepbuf\n\n%s was opened\n", ofile );
  }
}
// Changing the buffer variable names to match the ones in this program
int getch_(void) {
  char c = (bufp >= 0) ? buf[--bufp] : getchar();
  lastc = c & 0177;
//  if (lastc == '\n') {  // uncomment if you want to see the chars
//    printf("getch(): newline\n");
//  } else {
/     printf("getch_(): %c\n", lastc);
// }
  return lastc;
}
//professor functions
void ungetch_(int c) {
  if (bufp >= BUFSIZE) {
    printf("ungetch: overflow\n");
  }  else {
    genbuf[bufp++] = c;
  }
}
//professor functions
void search(const char* re) {
  char buf[GBSIZE];
  snprintf(buf, sizeof(buf), "/%s\n", re);  // / and \n very important
  //drawline();
  printf("g%s", buf);  const char* p = buf + strlen(buf) - 1;
  while (p >= buf) { ungetch_(*p--); }  global(1);
}
//=============================================================================
int main(int argc, char *argv[]) {
	zero = (unsigned *)malloc(nlall * sizeof(unsigned));  tfname = mktemp(tmpXXXXX);  init();  // Keep this the rest i might be able to comment out and rewrite

 if(argc > 1){
	 if (strcmp(argv[1] , "e") == 0)
	 {
		 printf("Called function readfile_ successfully\n");
		 readfile_(argv[2], "r");
	 }
	 else {  printf("Error changing this main\n" );}
 }
	commands();
	exfile();
	quit(0);
	return 0;
}
void commands(void) {
	unsigned int *a1;
	int c;
	int temp;
	char lastsep;
	for (;;) {
	if (pflag) {
		pflag = 0;
		addr1 = addr2 = dot;
		print();
	}//What is this default for?
	}
		default:
	// 		// if (nextopand == opcnt) {
	// 		// 	a += sign;
	// 		// 	if (a<zero || dol<a)
	// 		// 		continue;       /* error(Q); */		}
	// 		// if (c!='+' && c!='-' && c!='^') {
	// 		// 	peekc = c;
	// 		// 	if (opcnt==0)
	// 		// 		a = 0;
	// 		// 	return (a);	}
	// 		// sign = 1;
	// 		// if (c!='+'){sign = -sign;}
	// 		// nextopand = ++opcnt;
	// 		continue;
	// 	}
	// 	sign = 1;
	// 	opcnt++;
 	// } while (zero<=a && a<=dol);
	error(Q);
	/*NOTREACHED*/
	return 0;
}
int getnum(void) {
	int r, c;
	r = 0;
	while ((c=getchr())>='0' && c<='9')
		r = r*10 + c - '0';
	peekc = c;
	return (r);
}
void nonzero(void) {
	squeeze(1);
}
void squeeze(int i) {
	if (addr1<zero+i || addr2>dol || addr1>addr2)
		error(Q);
}
void newline(void) {
	int c;
	if ((c = getchr()) == '\n' || c == EOF)	{return;}
	if (c=='p' || c=='l' || c=='n') {
		pflag++;
		if (c=='l')		{	listf++;}
		else if (c=='n')	{listn++;}
		if ((c=getchr())=='\n')		{return;}
	}
	error(Q);
}
void filename(int comm) {
	char *p1, *p2;
	int c;
	count = 0;
	c = getchr();
	if (c=='\n' || c==EOF) {
		p1 = savedfile;
		if (*p1==0 && comm!='f')	{error(Q);}
		p2 = file;
		while ((*p2++ = *p1++))	{;}
		return;
	}
	if (c!=' ')	{error(Q);}
	while ((c = getchr()) == ' ')	{;}
	if (c=='\n')	{error(Q);}
	p1 = file;
	do {
		if (p1 >= &file[sizeof(file)-1] || c==' ' || c==EOF)
			error(Q);
		*p1++ = c;
	} while ((c = getchr()) != '\n');
	*p1++ = 0;
	if (savedfile[0]==0 || comm=='e' || comm=='f') {
		p1 = savedfile;
		p2 = file;
		while ((*p1++ = *p2++)
	)	{;}
	}
}
void exfile(void) {
	close(io);
	io = -1;
	if (vflag) {
		putd();
		putchr('\n');
	}
}
void error(char *s) {
	int c;
	wrapp = 0;
	listf = 0;
	listn = 0;
	putchr('?');
	puts(s);
	count = 0;
	lseek(0, (long)0, 2);
	pflag = 0;
	if (globp)
		lastc = '\n';
	globp = 0;
	peekc = lastc;
	if(lastc)
		while ((c = getchr()) != '\n' && c != EOF)
			;
	if (io > 0) {
		close(io);
		io = -1;
	}
}
int getchr(void) {
	getch_();
}
int getfile(void) {
	int c;
	char *lp, *fp;
	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0)
				if (lp>linebuf) {
					puts("'\\n' appended");
					*genbuf = '\n';
				}
				else {return(EOF);}
			fp = genbuf;
			while(fp < &genbuf[ninbuf]) {
				if (*fp++ & 0200){break;}
			}
			fp = genbuf;
		}
		c = *fp++;
		if (c=='\0')
			continue;
		if (c&0200 || lp >= &linebuf[LBSIZE]) {
			lastc = '\n';
			error(Q);
		}
		*lp++ = c;
		count++;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	return(0);
}
// Exits the program?
void quit(int n) {
	if (vflag && fchange && dol!=zero) {
		fchange = 0;
		error(Q);
	}
	unlink(tfname);
	exit(0);
}
void rdelete(unsigned int *ad1, unsigned int *ad2) {
	unsigned int *a1, *a2, *a3;
	a1 = ad1;
	a2 = ad2+1;
	a3 = dol;
	dol -= a2 - a1;
	do {
		*a1++ = *a2++;
	} while (a2 <= a3);
	a1 = ad1;
	if (a1 > dol)
		a1 = dol;
	dot = a1;
	fchange = 1;
}
void gdelete(void) {
	unsigned int *a1, *a2, *a3;
	a3 = dol;
	for (a1=zero; (*a1&01)==0; a1++)
		if (a1>=a3)
			return;
	for (a2=a1+1; a2<=a3;) {
		if (*a2&01) {
			a2++;
			dot = a1;
		} else
			*a1++ = *a2++;
	}
	dol = a1-1;
	if (dot>dol)
		dot = dol;
	fchange = 1;
}
char *getline(unsigned int tl) {
	char *bp, *lp;
	int nl;
	lp = linebuf;
	bp = getblock(tl, READ);
	nl = nleft;
	tl &= ~((BLKSIZE/2)-1);
	while ((*lp++ = *bp++))
		if (--nl == 0) {
			bp = getblock(tl+=(BLKSIZE/2), READ);
			nl = nleft;
		}
	return(linebuf);
}
int putline(void) {
	char *bp, *lp;
	int nl;
	unsigned int tl;
	fchange = 1;
	lp = linebuf;
	tl = tline;
	bp = getblock(tl, WRITE);
	nl = nleft;
	tl &= ~((BLKSIZE/2)-1);
	while ((*bp = *lp++)) {
		if (*bp++ == '\n') {
			*--bp = 0;
			linebp = lp;
			break;
		}
		if (--nl == 0) {
			bp = getblock(tl+=(BLKSIZE/2), WRITE);
			nl = nleft;
		}
	}
	nl = tline;
	tline += (((lp-linebuf)+03)>>1)&077776;
	return(nl);
}
void init(void) {
	int *markp;
	close(tfile);
	tline = 2;
	for (markp = names; markp < &names[26]; )
		*markp++ = 0;
	subnewa = 0;
	anymarks = 0;
	iblock = -1;
	oblock = -1;
	ichanged = 0;
	close(creat(tfname, 0600));
	tfile = open(tfname, 2);
	dot = dol = zero;
}
void global(int k) {
	char *gp;
	int c;
	unsigned int *a1;
	char globuf[GBSIZE];
	if (globp)
		error(Q);
	setwide();
	squeeze(dol>zero);
	if ((c=getchr())=='\n')
		error(Q);
	compile(c);
	gp = globuf;
	while ((c = getchr()) != '\n') {
		if (c==EOF)
			error(Q);
		if (c=='\\') {
			c = getchr();
			if (c!='\n')
				*gp++ = '\\';
		}
		*gp++ = c;
		if (gp >= &globuf[GBSIZE-2])
			error(Q);
	}
	if (gp == globuf)
		*gp++ = 'p';
	*gp++ = '\n';
	*gp++ = 0;
	for (a1=zero; a1<=dol; a1++) {
		*a1 &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(a1)==k)
			*a1 |= 01;
	}
	/*
	 * Special case: g/.../d (avoid n^2 algorithm)
	 */
	if (globuf[0]=='d' && globuf[1]=='\n' && globuf[2]=='\0') {
		gdelete();
		return;
	}
	for (a1=zero; a1<=dol; a1++) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			globp = globuf;
			commands();
			a1 = zero;
		}
	}
}
//					Important Do Not delete?
void join(void) {
	char *gp, *lp;
	unsigned int *a1;
	nonzero();
	gp = genbuf;
	for (a1=addr1; a1<=addr2; a1++) {
		lp = getline(*a1);
		while ((*gp = *lp++))
			if (gp++ >= &genbuf[LBSIZE-2])
				error(Q);
	}
	lp = linebuf;
	gp = genbuf;
	while ((*lp++ = *gp++))
		;
	*addr1 = putline();
	if (addr1<addr2)
		rdelete(addr1+1, addr2);
	dot = addr1;
}
int compsub(void) {
	int seof, c;
	char *p;
	if ((seof = getchr()) == '\n' || seof == ' ')
		error(Q);
	compile(seof);
	p = rhsbuf;
	for (;;) {
		c = getchr();
		if (c=='\\')
			c = getchr() | 0200;
		if (c=='\n') {
			if (globp && globp[0])	/* last '\n' does not count */
				c |= 0200;
			else {
				peekc = c;
				pflag++;
				break;
			}
		}
		if (c==seof)
			break;
		*p++ = c;
		if (p >= &rhsbuf[LBSIZE/2])
			error(Q);
	}
	*p++ = 0;
	if ((peekc = getchr()) == 'g') {
		peekc = 0;
		newline();
		return(1);
	}
	newline();
	return(0);
}
char *place(char *sp, char *l1, char *l2) {
	while (l1 < l2) {
		*sp++ = *l1++;
		if (sp >= &genbuf[LBSIZE])
			error(Q);
	}
	return(sp);
}
int getcopy(void) {
	if (addr1 > addr2)
		return(EOF);
	getline(*addr1++);
	return(0);
}
//					Important Do Not delete?
// int advance(char *lp, char *ep) {
// 	char *curlp;
// 	int i;
// 	for (;;) switch (*ep++) {
// 	case CCHR:
// 		if (*ep++ == *lp++)
// 			continue;
// 		return(0);
// 	case CDOT:
// 		if (*lp++){continue;}
// 		return(0);
// 	case CDOL:
// 		if (*lp==0){continue;}
// 		return(0);
// 	case CEOF:
// 		loc2 = lp;
// 		return(1);
// 	case CCL:
// 		if (cclass(ep, *lp++, 1)) {
// 			ep += *ep;
// 			continue;
// 		}
// 		return(0);
// 	case NCCL:
// 		if (cclass(ep, *lp++, 0)) {
// 			ep += *ep;
// 			continue;
// 		}
// 		return(0);
// 	case CBRA:	braslist[*ep++] = lp;		continue;
// 	case CKET:	braelist[*ep++] = lp;		continue;
// 	case CBACK:
// 		if (braelist[i = *ep++]==0)
// 			error(Q);
// 		if (backref(i, lp)) {
// 			lp += braelist[i] - braslist[i];
// 			continue;
// 		}
// 		return(0);
// 	case CBACK|STAR:
// 		if (braelist[i = *ep++] == 0)
// 			error(Q);
// 		curlp = lp;
// 		while (backref(i, lp))
// 			lp += braelist[i] - braslist[i];
// 		while (lp >= curlp) {
// 			if (advance(lp, ep))
// 				return(1);
// 			lp -= braelist[i] - braslist[i];
// 		}	continue;
// 	case CDOT|STAR:
// 		curlp = lp;
// 		while (*lp++){;}
// 		goto star;
// 	case CCHR|STAR:
// 		curlp = lp;
// 		while (*lp++ == *ep){;}
// 		ep++;
// 		goto star;
// 	case CCL|STAR:
// 	case NCCL|STAR:
// 		curlp = lp;
// 		while (cclass(ep, *lp++, ep[-1]==(CCL|STAR))){	;}
// 		ep += *ep;
// 		goto star;
// 	star:
// 		do {
// 			lp--;
// 			if (advance(lp, ep))
// 				return(1);
// 		} while (lp > curlp);
// 		return(0);
// 	default:
// 		error(Q);
// 	}
// }
void puts(char *sp) {
	col = 0;
	while (*sp)
		putchr(*sp++);
	putchr('\n');
}
char	line[70];
char	*linp	= line;
void putchr(int ac) {
	char *lp;
		int c;
	lp = linp;
	c = ac;
	if (listf) {
		if (c=='\n') {
			if (linp!=line && linp[-1]==' ') {
				*lp++ = '\\';
				*lp++ = 'n';
			}
		} else {
			if (col > (72-4-2)) {
				col = 8;
				*lp++ = '\\';
				*lp++ = '\n';
				*lp++ = '\t';
			}
			col++;
			if (c=='\b' || c=='\t' || c=='\\') {
				*lp++ = '\\';
				if (c=='\b')
					c = 'b';
				else if (c=='\t')
					c = 't';
				col++;
			} else if (c<' ' || c=='\177') {
				*lp++ = '\\';
				*lp++ =  (c>>6)    +'0';
				*lp++ = ((c>>3)&07)+'0';
				c     = ( c    &07)+'0';
				col += 3;
			}
		}
	}
	*lp++ = c;
	if(c == '\n' || lp >= &line[64]) {
		linp = line;
		write(oflag?2:1, line, lp-line);
		return;
	}
	linp = lp;
}
