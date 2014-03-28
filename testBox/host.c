#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>

#define l_SERVER     'ldaps://ldap.sfc.keio.ac.jp'
#define BASE_DN      "ou=people,dc=sfc,dc=keio,dc=ac,dc=jp"

int main()
{
	
    LDAP        *ld;
    LDAPMessage *result=NULL;
	int          entries;
	char ll[64]="ldap.sfc.keio.ac.jp";
	const char *hostname=ll;
	
	char *base=BASE_DN;
	char str[80];
	char *attrs[2]={"keioIDNumber",0};
	
	// Initialize
	ld = ldap_init(hostname, 389);
	if (ld == NULL) {
        perror("ldap_init");
        exit(1);
	}
	
    if (ldap_simple_bind_s(ld, NULL, NULL) != LDAP_SUCCESS) {
        ldap_perror(ld, "ldap_simple_bind_s");
        ldap_unbind(ld);
        exit(1);
	}
	
	scanf("%s",str);
	char attr[100]="(uid=";
	strcat(attr,str);
	strcat(attr,")");
	printf("%s\n",attr);
	char *a=attr;
	
	// Search Entry
    if (ldap_search_s(ld, base, LDAP_SCOPE_SUBTREE,
        a, 0, 0, &result) != LDAP_SUCCESS)
    {
        ldap_perror(ld, "ldap_search_s");
        ldap_unbind(ld);
        exit(1);
    }

    entries = ldap_count_entries(ld, result);
    printf("Found %d entries\n", entries);
	
	result=ldap_first_entry(ld,result);
	int i,lim;
	char **vals;
	char *num;
	if(result==NULL){
		ldap_perror(ld,"ldap_first_entry");
		ldap_unbind(ld);
		exit(1);
	}else{
		while(result!=NULL){
			vals=ldap_get_values(ld,result,"keioIDNumber");
			lim=ldap_count_values(vals);
			for(i=0;i<lim;i++)printf("ID:%s\n",vals[i]);
			result=ldap_next_entry(ld,result);
		}
		ldap_value_free(vals);
	}
    ldap_msgfree(result);
	ldap_unbind(ld);
}