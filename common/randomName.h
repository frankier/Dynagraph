// tricky problem: if client doesn't specify a name, we need to generate one
// can't use e.g. e17 because the client might already have something named that!
// so generate a random name
inline DString randomName(char prefix) {
	char buf[10],i=2;
	unsigned char ch;
	buf[0] = prefix;
	buf[1] = '_';
	while(i<8) 
		if(isalnum(ch = rand()%256))
			buf[i++] = ch;
	buf[8] = 0;
	return buf;
}
// to detect those bad names that agraph is generating
inline bool poorEdgeName(const char *name) {
	if(!name[0])
		return true;
	if(name[0]!='e')
		return false;
	if(!name[1])
		return false; // "e" okay i guess
	for(int i=1;name[i]; ++i)
		if(!isdigit(name[i]))
			return false;
	return true;
}
