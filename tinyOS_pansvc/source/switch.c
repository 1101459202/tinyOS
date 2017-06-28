
__asm void PendSV_Handler(void)
{
	IMPORT blockPtr
	
	LDR r0,=blockPtr  ;将blockPtr "变量的地址" 加载到r0
	LDR r0,[R0]				;将blockPtr值加载到r0   将R0 存储器的值 放到r0  [R0]相当于*R0
	LDR r0,[R0]				;将stackPtr值加载到r0
	
	STMDB r0!,{r4-r11}
	
	;更新栈的位置
	LDR r1,=blockPtr	;将blockPtr变量的地址加载到r1
  LDR r1,[r1]				;将blockPtr的值加载到r1
	STR r0,[r1]				;将r0的值加载到stackPtr
	
	ADD r4,r4,#1
	ADD r5,r5,#1
	
	LDMIA r0!,{r4-r11}
	
	BX LR
}
