
__asm void PendSV_Handler(void)
{
	IMPORT blockPtr
	
	LDR r0,=blockPtr  ;��blockPtr "�����ĵ�ַ" ���ص�r0
	LDR r0,[R0]				;��blockPtrֵ���ص�r0   ��R0 �洢����ֵ �ŵ�r0  [R0]�൱��*R0
	LDR r0,[R0]				;��stackPtrֵ���ص�r0
	
	STMDB r0!,{r4-r11}
	
	;����ջ��λ��
	LDR r1,=blockPtr	;��blockPtr�����ĵ�ַ���ص�r1
  LDR r1,[r1]				;��blockPtr��ֵ���ص�r1
	STR r0,[r1]				;��r0��ֵ���ص�stackPtr
	
	ADD r4,r4,#1
	ADD r5,r5,#1
	
	LDMIA r0!,{r4-r11}
	
	BX LR
}
