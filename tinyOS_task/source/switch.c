
__asm void PendSV_Handler(void)
{
	IMPORT blockPtr
	
	LDR r0,=blockPtr
	LDR r0,[R0]
	LDR r0,[R0]
	
	STMDB r0!,{r4-r11}
	
	LDR r1,=blockPtr
  LDR r1,[r1]
	STR r0,[r1]
	
	ADD r4,r4,#1
	ADD r5,r5,#1
	
	LDMIA r0!,{r4-r11}
	
	BX LR
}