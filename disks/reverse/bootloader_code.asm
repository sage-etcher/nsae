			org	57098
57098			jp	labDF0D
57101		labDF0D:
57101			di	
57102			ld	sp,61660
57105			ld	hl,labE6FF
57108			ld	a,(hl)
57109			push	af
57110			ld	(hl),0
57112			ld	bc,129
57115			ld	de,2049
57118			ld	hl,labE6FF+1
57121			ld	a,1
57123			call	labE033
57126			ld	bc,129
57129			ld	de,257
57132			ld	hl,59648
57135			ld	a,3
57137			call	labE033
57140			call	labE2B4
57143			ld	bc,8
57146		labDF3A:
57146			push	bc	
57147			ld	c,0
57149			call	59657
57152			ld	bc,16383
57155			ld	h,b
57156			ld	l,c
57157			ld	d,h
57158			ld	e,l
57159			lddr
57161			pop	bc
57162			ld	a,(hl)
57163			cpl
57164			ld	(hl),a
57165			cp	(hl)
57166			cpl
57167			ld	(hl),a
57168			jr	nz,labDF56
57170			inc	b
57171			dec	c
57172			jr	nz,labDF3A
57174		labDF56:
57174			ld	a,3	
57176			out	(96),a
57178			ld	bc,256
57181			call	59657
57184			ld	bc,513
57187			call	59657
57190			ld	bc,770
57193			call	59657
57196			ld	bc,5
57199			call	59657
57202			ld	hl,61027
57205			ld	(61183),hl
57208			ld	a,238
57210			ld	i,a
57212			im	2
57214			pop	af
57215			ld	sp,256
57218			ld	(labE6FF),a
57221			ld	c,4
57223			call	59654
57226			set	7,b
57228			call	59657
57231			call	59974
57234			ld	c,4
57236			call	labE572
57239			ld	hl,labDFA9
57242			call	59927
57245			jp	59862
57248			nop
57249			nop
57250			nop
57251			nop
57252			nop
57253			nop
57254			nop
57255			nop
57256			nop
57257		labDFA9:
57257			ld	(hl),52	
57259			ld	c,e
57260			jr	nz,labDFF5
57262			ld	(hl),d
57263			ld	h,c
57264			ld	(hl),b
57265			ld	l,b
57266			ld	l,c
57267			ld	h,e
57268			ld	(hl),e
57269			jr	nz,labDFFA
57271			ld	d,b
57272			cpl
57273			ld	c,l
57274			jr	nz,labDFEE
57276			ld	l,50
57278			jr	nz,labDFDF+1
57280			ld	(hl),d
57281			ld	h,l
57282			halt
57283			jr	nz,labDFF6
57285			ld	l,50
57287			ld	l,48
57289			jr	nz,labE030+1
57291			ld	l,a
57292			ld	(hl),d
57293			jr	nz,labE00F+1
57295			ld	h,h
57296			halt
57297			ld	h,c
57298			ld	l,(hl)
57299			ld	(hl),h
57300			ld	h,c
57301			ld	h,a
57302			ld	h,l
57303			jr	nz,labE021
57305			ld	d,c
57306			jr	nz,labE043-1
57308			ld	(hl),d
57309			ld	l,a
57310			ld	l,l
57311		labDFDF:
57311			jr	nz,labE02D+2	
57313			ld	l,a
57314			ld	(hl),d
57315			ld	(hl),h
57316			ld	l,b
57317			jr	nz,labE039+1
57319			ld	(hl),h
57320			ld	h,c
57321			ld	(hl),d
57322			jr	nz,labE02D+2
57324			ld	l,a
57325			ld	l,l
57326		labDFEE:
57326			ld	(hl),b	
57327			ld	(hl),l
57328			ld	(hl),h
57329			ld	h,l
57330			ld	(hl),d
57331			ld	(hl),e
57332			inc	l
57333		labDFF5:
57333			ld	c,c	
57334		labDFF6:
57334			ld	l,(hl)	
57335			ld	h,e
57336			ld	l,13
57338		labDFFA:
57338			ld	a,(bc)	
57339			ld	a,(bc)
57340			rrca
57341			inc	e
57342			add	a,b
57343			nop
57344			jp	60994
57347		labE003:
57347			jp	59758	
57350			jp	labE6FF+4
57353		labE009:
57353			jp	labE6FF+7	
57356			jp	labE56D
57359		labE00F:
57359			jp	59148	
57362			jp	59151
57365			jp	59154
57368			jp	labE556
57371			jp	labE3E2
57374			jp	labE559
57377		labE021:
57377			jp	labE55F	
57380			jp	labE564
57383			jp	labE575
57386			jp	labE583
57389		labE02D:
57389			jp	59157	
57392		labE030:
57392			jp	labE56A	
57395		labE033:
57395			jp	labE347	
57398		labE036:
57398			ret	
57399			nop
57400			nop
57401		labE039:
57401			jp	labE039	
57404			jp	labE642
57407		labE03F:
57407			ld	(61562),sp	
57411		labE043:
57411			ld	sp,61616	
57414			push	af
57415			push	hl
57416			push	de
57417			ld	a,5
57419			bit	7,c
57421			jp	z,labE115
57424			ld	a,c
57425			and	7
57427			ld	c,a
57428			cp	3
57430			jr	c,labE05B
57432			rla
57433			and	12
57435		labE05B:
57435			or	128	
57437			ld	l,a
57438			ld	a,(labE6F5+1)
57441			and	l
57442			jr	z,labE075
57444			ld	e,40
57446			ld	a,b
57447			add	a,a
57448			add	a,a
57449			add	a,a
57450			add	a,a
57451			or	d
57452			ld	d,a
57453			rr	b
57455			jr	nc,labE08B
57457			set	6,l
57459			jr	labE08B
57461		labE075:
57461			ld	e,20	
57463			ld	a,34
57465			sub	b
57466			jr	nc,labE081
57468			add	a,35
57470			ld	b,a
57471			set	6,l
57473		labE081:
57473			ld	a,l	
57474			and	64
57476			or	b
57477			rrca
57478			rrca
57479			and	240
57481			add	a,d
57482			ld	d,a
57483		labE08B:
57483			ld	a,d	
57484			ld	(61560),a
57487			ld	a,e
57488			cp	b
57489			jr	nc,labE095
57491			set	5,l
57493		labE095:
57493			ld	a,l	
57494			ld	(61556),a
57497			di
57498			ld	de,(59666)
57502		labE09E:
57502			call	labE1FB	
57505			ld	a,(labE6BC)
57508			and	127
57510			jr	nz,labE09E
57512			push	bc
57513			call	labE28A
57516			ld	a,(61556)
57519			out	(129),a
57521			pop	bc
57522			pop	hl
57523			ld	a,l
57524			or	a
57525			jr	nz,labE0C0
57527			in	a,(224)
57529			and	16
57531			ld	a,6
57533			jp	nz,labE111
57536		labE0C0:
57536			push	hl	
57537			push	bc
57538			ld	hl,labE6E1
57541			ld	b,0
57543			add	hl,bc
57544			ld	(labE6E6),hl
57547			ld	a,(hl)
57548			xor	89
57550			push	hl
57551			call	z,labE2F3
57554			pop	hl
57555			pop	af
57556			call	labE2FC
57559			ld	a,(61556)
57562			out	(129),a
57564			call	labE2E1
57567			pop	bc
57568			pop	hl
57569			or	a
57570		labE0E2:
57570			push	bc	
57571			push	hl
57572		labE0E4:
57572			call	nz,labE25F	
57575			cp	b
57576			push	af
57577			in	a,(224)
57579			and	72
57581			jr	nz,labE0FC
57583			call	labE21F
57586			pop	af
57587			jr	nz,labE0E4
57589			dec	c
57590			jp	m,labE1AF
57593			jp	labE128
57596		labE0FC:
57596			call	labE2A9	
57599			pop	af
57600			inc	a
57601			jr	labE0E4
57603		labE103:
57603			pop	bc	
57604			inc	b
57605			ld	a,(61560)
57608			inc	a
57609			ld	(61560),a
57612			pop	af
57613			dec	a
57614			push	af
57615			jr	nz,labE0E2
57617		labE111:
57617			ld	(59666),de	
57621		labE115:
57621			ld	sp,61614	
57624			push	af
57625			ex	de,hl
57626			call	labE33F
57629			ex	de,hl
57630			in	a,(130)
57632			pop	af
57633			pop	bc
57634			or	a
57635			ld	sp,(61562)
57639			ret
57640		labE128:
57640			push	bc	
57641			ld	hl,35
57644			call	labE207
57647			ld	a,(labE6BF+2)
57650			and	247
57652			out	(248),a
57654			out	(130),a
57656			ld	hl,9
57659			call	labE207
57662			ld	a,(labE6BF+2)
57665			out	(248),a
57667			pop	bc
57668			dec	c
57669			ld	hl,labE171
57672			jr	nz,labE14D
57674			ld	hl,labE191
57677		labE14D:
57677			ex	(sp),hl	
57678			call	labE26C
57681			in	a,(129)
57683			cp	251
57685			jp	nz,labE285
57688			ld	a,(61560)
57691			ld	b,a
57692			in	a,(128)
57694			cp	b
57695			ld	(61561),a
57698			ld	bc,0
57701			ret	z
57702			cp	251
57704			ret	z
57705			ld	hl,(labE6E6)
57708			ld	(hl),89
57710			jp	labE1AA
57713		labE171:
57713			in	a,(128)	
57715			ld	(hl),a
57716			xor	c
57717			rlca
57718			ld	c,a
57719			call	labE1E1
57722			in	a,(128)
57724			inc	hl
57725			ld	(hl),a
57726			inc	hl
57727			xor	c
57728			rlca
57729			ld	c,a
57730			djnz	labE171
57732		labE184:
57732			in	a,(128)	
57734			xor	c
57735			in	a,(130)
57737			jp	z,labE103
57740			ld	a,2
57742			jp	labE111
57745		labE191:
57745			in	a,(128)	
57747			cp	(hl)
57748			jr	nz,labE1AA
57750			xor	c
57751			rlca
57752			ld	c,a
57753			call	labE1E1
57756			in	a,(128)
57758			inc	hl
57759			cp	(hl)
57760			jr	nz,labE1AA
57762			xor	c
57763			rlca
57764			ld	c,a
57765			inc	hl
57766			djnz	labE191
57768			jr	labE184
57770		labE1AA:
57770			ld	a,3	
57772			jp	labE111
57775		labE1AF:
57775			ld	bc,8704	
57778			out	(131),a
57780		labE1B4:
57780			xor	a	
57781			out	(128),a
57783			call	labE1E1
57786			djnz	labE1B4
57788			ld	a,251
57790			out	(128),a
57792			ex	(sp),hl
57793			ex	(sp),hl
57794			ld	a,(61560)
57797			out	(128),a
57799			pop	hl
57800		labE1C8:
57800			ld	a,(hl)	
57801			inc	hl
57802			out	(128),a
57804			xor	c
57805			rlca
57806			ld	c,a
57807			call	labE1E1
57810			ld	a,(hl)
57811			out	(128),a
57813			inc	hl
57814			xor	c
57815			rlca
57816			ld	c,a
57817			djnz	labE1C8
57819			ld	a,c
57820			out	(128),a
57822			jp	labE103
57825		labE1E1:
57825			in	a,(224)	
57827			and	0
57829			ret	z
57830			in	a,(0)
57832			ld	(de),a
57833			inc	de
57834			ret
57835			push	af
57836			push	de
57837			ld	de,(59666)
57841			call	labE1E1
57844			ld	(59666),de
57848			pop	de
57849			pop	af
57850			ret
57851		labE1FB:
57851			ld	(59666),de	
57855			ret
57856			ld	(hl),c
57857			xor	237
57859			ld	e,e
57860			ld	(de),a
57861			jp	(hl)
57862			ret
57863		labE207:
57863			ld	bc,65535	
57866		labE20A:
57866			in	a,(224)	
57868			and	0
57870			jp	z,labE21A
57873			add	hl,bc
57874			jp	nc,labE21E
57877			in	a,(0)
57879			ld	(de),a
57880			inc	de
57881			or	a
57882		labE21A:
57882			add	hl,bc	
57883			jp	c,labE20A
57886		labE21E:
57886			ret	
57887		labE21F:
57887			ld	hl,408	
57890		labE222:
57890			in	a,(224)	
57892			and	64
57894			ret	nz
57895			in	a,(224)
57897			and	0
57899			jp	z,labE237
57902			in	a,(224)
57904			and	64
57906			ret	nz
57907			in	a,(0)
57909			ld	(de),a
57910			inc	de
57911		labE237:
57911			in	a,(224)	
57913			and	64
57915			ret	nz
57916			dec	hl
57917			ld	a,h
57918			or	l
57919			jp	nz,labE222
57922		labE242:
57922			ld	(labE6E1),a	
57925			ld	a,4
57927			jp	labE111
57930		labE24A:
57930			ld	hl,1263	
57933		labE24D:
57933			call	labE1E1	
57936			in	a,(224)
57938			and	64
57940			ret	z
57941			dec	hl
57942			ld	a,h
57943			or	l
57944			jr	nz,labE24D
57946			jr	labE242
57948		labE25C:
57948			call	labE21F	
57951		labE25F:
57951			call	labE1FB	
57954			call	labE24A
57957			call	labE2D6
57960			inc	a
57961			and	15
57963			ret
57964		labE26C:
57964			ld	bc,13792	
57967		labE26F:
57967			in	f,(c)	
57969			ret	m
57970			in	a,(224)
57972			and	0
57974			jp	z,labE280
57977			in	f,(c)
57979			ret	m
57980			in	a,(0)
57982			ld	(de),a
57983			inc	de
57984		labE280:
57984			in	f,(c)	
57986			ret	m
57987			djnz	labE26F
57989		labE285:
57989			ld	a,1	
57991			jp	labE111
57994		labE28A:
57994			call	labE2A9	
57997			cp	14
57999			ret	nz
58000			xor	a
58001			ld	(labE6E1),a
58004			call	labE2AF
58007			ld	b,100
58009		labE299:
58009			push	hl	
58010		labE29A:
58010			push	bc	
58011			call	labE1FB
58014			ld	hl,406
58017			call	labE207
58020			pop	bc
58021			djnz	labE29A
58023			pop	hl
58024			ret
58025		labE2A9:
58025			call	labE2B4	
58028			cp	14
58030			ret	z
58031		labE2AF:
58031			ld	h,5	
58033			call	labE2B6
58036		labE2B4:
58036			ld	h,0	
58038		labE2B6:
58038			call	labE1E1	
58041			ld	a,(labE6BF+2)
58044			ld	l,a
58045			and	248
58047			or	h
58048			ld	h,a
58049			cp	l
58050			jr	z,labE2D6
58052			in	a,(208)
58054			ld	l,a
58055			ld	a,h
58056			out	(248),a
58058			ld	(labE6BF+2),a
58061		labE2CD:
58061			call	labE1E1	
58064			in	a,(208)
58066			xor	l
58067			jp	p,labE2CD
58070		labE2D6:
58070			ld	l,128	
58072		labE2D8:
58072			in	a,(208)	
58074			and	15
58076			cp	l
58077			ret	z
58078			ld	l,a
58079			jr	labE2D8
58081		labE2E1:
58081			ld	hl,labE6E1	
58084			ld	a,(hl)
58085			ld	(hl),c
58086			cp	c
58087			ret	z
58088			ld	b,12
58090			call	labE299
58093		labE2ED:
58093			call	labE25C	
58096			ret	z
58097			jr	labE2ED
58099		labE2F3:
58099			push	hl	
58100			call	labE2E1
58103			call	labE21F
58106			pop	hl
58107			xor	a
58108		labE2FC:
58108			ld	b,a	
58109			sub	(hl)
58110			ld	(hl),b
58111			ret	z
58112			ld	l,32
58114			ld	h,a
58115			jp	p,labE310
58118			cpl
58119			inc	a
58120			ld	h,a
58121			in	a,(224)
58123			and	32
58125			ret	nz
58126			ld	l,0
58128		labE310:
58128			ld	a,(61556)	
58131			and	207
58133			or	l
58134			ld	l,a
58135		labE317:
58135			ld	a,l	
58136			out	(129),a
58138			or	16
58140			out	(129),a
58142			xor	16
58144			out	(129),a
58146			ld	a,(labE6F8)
58149			and	l
58150			and	15
58152			ld	b,1
58154			jr	nz,labE32E
58156			ld	b,5
58158		labE32E:
58158			call	labE299	
58161			in	a,(224)
58163			and	32
58165			jr	nz,labE33A
58167			dec	h
58168			jr	nz,labE317
58170		labE33A:
58170			ld	b,3	
58172			jp	labE299
58175		labE33F:
58175			ld	hl,labE6FF	
58178			bit	3,(hl)
58180			ret	z
58181			ei
58182			ret
58183		labE347:
58183			ld	(61558),a	
58186		labE34A:
58186			ld	a,(labE6E9)	
58189		labE34D:
58189			ld	(61559),a	
58192			push	hl
58193			push	de
58194			push	bc
58195			ld	a,(61558)
58198			and	15
58200			call	labE03F
58203			pop	bc
58204			pop	de
58205			pop	hl
58206			jr	nz,labE372
58208			ld	a,e
58209			or	a
58210			jr	nz,labE370
58212			ld	a,(labE6FF)
58215			and	64
58217			ld	e,2
58219			jr	nz,labE34A
58221			call	labE25C
58224		labE370:
58224			xor	a	
58225			ret
58226		labE372:
58226			ld	(labE3D2+1),a	
58229			cp	4
58231			jr	nc,labE37F
58233			ld	a,(61559)
58236			dec	a
58237			jr	nz,labE34D
58239		labE37F:
58239			ld	a,c	
58240			and	15
58242			ld	(labE3D6+1),a
58245			ld	a,b
58246			push	hl
58247			push	de
58248			push	bc
58249			ld	hl,labE3DE
58252			ld	(hl),d
58253			dec	hl
58254			call	labE3A8
58257			ld	a,c
58258			call	labE3A8
58261			ld	(hl),c
58262			ld	hl,labE3CF
58265			ld	a,(labE3D2+1)
58268			call	labE3B4
58271			pop	bc
58272			pop	de
58273			pop	hl
58274			jp	nz,labE34A
58277			xor	a
58278			scf
58279			ret
58280		labE3A8:
58280			ld	c,255	
58282		labE3AA:
58282			sub	10	
58284			inc	c
58285			jr	nc,labE3AA
58287			add	a,10
58289			ld	(hl),a
58290			dec	hl
58291			ret
58292		labE3B4:
58292			or	a	
58293			call	labE6EA
58296			or	a
58297			call	nz,59927
58300			xor	a
58301			call	labE6EA
58304			or	a
58305			call	z,labE009
58308			cp	3
58310			jp	z,labE003
58313			ld	b,a
58314			ld	a,(labE6F9)
58317			cp	b
58318			ret
58319		labE3CF:
58319			dec	c	
58320			ld	a,(bc)
58321			ld	d,h
58322		labE3D2:
58322			jr	nz,labE3F3+1	
58324			jr	nz,labE41A
58326		labE3D6:
58326			jr	nz,labE3F7+1	
58328			jr	nz,labE42C+1
58330			jr	nz,labE3FC
58332			jr	nz,labE3FD+1
58334		labE3DE:
58334			nop	
58335			dec	c
58336			ld	a,(bc)
58337			add	a,b
58338		labE3E2:
58338			ld	l,c	
58339			ld	h,0
58341			xor	a
58342			call	labE036
58345			ld	(61550),hl
58348			ex	de,hl
58349			or	a
58350			ld	(61546),a
58353			ret	nz
58354			dec	a
58355		labE3F3:
58355			ld	(61555),a	
58358			ld	a,e
58359		labE3F7:
58359			cp	4	
58361			jp	nc,labE487
58364		labE3FC:
58364			ld	a,l	
58365		labE3FD:
58365			and	1	
58367			jp	nz,labE473
58370			call	labE642
58373			ld	a,255
58375			ld	(61544),a
58378			ld	a,(61550)
58381			call	labE48F
58384			or	128
58386			ld	c,a
58387		labE413:
58387			ld	b,0	
58389			ld	de,1
58392			ld	a,1
58394		labE41A:
58394			ld	hl,61696	
58397			call	labE033
58400			jr	c,labE413
58402			ld	b,144
58404			res	7,c
58406			ld	hl,61788
58409			ld	a,(hl)
58410			and	16
58412		labE42C:
58412			ld	de,labE516	
58415			jr	z,labE453
58417			ld	b,(hl)
58418			ld	a,b
58419			and	64
58421			jr	z,labE445
58423			ld	a,(labE6F8)
58426			push	bc
58427		labE43B:
58427			rlca	
58428			dec	c
58429			jr	nz,labE43B
58431			pop	bc
58432			ld	de,labE536
58435			jr	c,labE453
58437		labE445:
58437			ld	a,191	
58439			and	b
58440			ld	b,a
58441			ld	de,labE526
58444			and	32
58446			jr	nz,labE453
58448			ld	de,labE516
58451		labE453:
58451			ld	a,(labE6F5+1)	
58454			ld	a,b
58455			ld	hl,(61550)
58458			ld	bc,labE6F9+1
58461			add	hl,bc
58462			ld	(hl),a
58463			and	12
58465			ld	hl,0
58468			ret	nz
58469			call	labE473
58472			dec	hl
58473			ld	a,(de)
58474			ld	(hl),a
58475			inc	de
58476			ld	bc,11
58479			add	hl,bc
58480			ld	(hl),e
58481			inc	hl
58482			ld	(hl),d
58483		labE473:
58483			ld	a,(61550)	
58486			ld	hl,labE4C1
58489			ld	bc,17
58492			inc	a
58493		labE47D:
58493			add	hl,bc	
58494			dec	a
58495			jr	nz,labE47D
58497			ld	a,(hl)
58498			ld	(61546),a
58501			inc	hl
58502			ret
58503		labE487:
58503			ld	hl,0	
58506			xor	a
58507			ld	(4),a
58510			ret
58511		labE48F:
58511			dec	a	
58512			ld	hl,labE6E6+2
58515			cp	(hl)
58516			inc	a
58517			jr	c,labE4AA
58519			ld	hl,labE4B3+1
58522			add	a,65
58524			cp	(hl)
58525			jr	z,labE4A9
58527			ld	(hl),a
58528			ld	hl,labE4AC
58531			call	59927
58534			call	labE009
58537		labE4A9:
58537			xor	a	
58538		labE4AA:
58538			inc	a	
58539			ret
58540		labE4AC:
58540			dec	c	
58541			ld	a,(bc)
58542			ld	c,l
58543			ld	c,a
58544			ld	d,l
58545			ld	c,(hl)
58546			ld	d,h
58547		labE4B3:
58547			jr	nz,labE4F6	
58549			ld	a,(18720)
58552			ld	c,(hl)
58553			jr	nz,labE4FD+2
58555			ld	d,d
58556			ld	c,c
58557			ld	d,(hl)
58558			ld	b,l
58559			jr	nz,labE4F2
58561		labE4C1:
58561			inc	l	
58562			jr	nz,labE517+1
58564			ld	e,c
58565			ld	d,b
58566			ld	b,l
58567			jr	nz,labE50A
58569			ld	c,(hl)
58570			ld	e,c
58571			jr	nz,labE517+1
58573			ld	b,l
58574			ld	e,c
58575			dec	c
58576			ld	a,(bc)
58577			add	a,b
58578			nop
58579			nop
58580			nop
58581			nop
58582			nop
58583			nop
58584			nop
58585			nop
58586			nop
58587			ld	bc,9967
58590			push	hl
58591			sbc	a,d
58592			rst	40
58593			add	a,c
58594			rst	40
58595			nop
58596			nop
58597			nop
58598			nop
58599			nop
58600			nop
58601			nop
58602			nop
58603			nop
58604			ld	bc,9967
58607			push	hl
58608			out	(239),a
58610		labE4F2:
58610			cp	d	
58611			rst	40
58612			nop
58613			nop
58614		labE4F6:
58614			nop	
58615			nop
58616			nop
58617			nop
58618			nop
58619			nop
58620			nop
58621		labE4FD:
58621			ld	bc,9967	
58624			push	hl
58625			inc	c
58626			ret	p
58627			di
58628			rst	40
58629			nop
58630			nop
58631			nop
58632			nop
58633			nop
58634		labE50A:
58634			nop	
58635			nop
58636			nop
58637			nop
58638			ld	bc,9967
58641			push	hl
58642			ld	b,l
58643			ret	p
58644			inc	l
58645			ret	p
58646		labE516:
58646			ex	af,af'	
58647		labE517:
58647			jr	z,labE519	
58649		labE519:
58649			inc	bc	
58650			rlca
58651			nop
58652			and	h
58653			nop
58654			ccf
58655			nop
58656			ret	nz
58657			nop
58658			djnz	labE524
58660		labE524:
58660			ld	(bc),a	
58661			nop
58662		labE526:
58662			djnz	labE550	
58664			nop
58665			inc	b
58666			rrca
58667			ld	bc,81
58670			ccf
58671			nop
58672			add	a,b
58673			nop
58674			djnz	labE534
58676		labE534:
58676			ld	(bc),a	
58677			nop
58678		labE536:
58678			djnz	labE560	
58680			nop
58681			inc	b
58682			rrca
58683			ld	bc,169
58686			ccf
58687			nop
58688			add	a,b
58689			nop
58690			djnz	labE544
58692		labE544:
58692			ld	(bc),a	
58693			nop
58694			jr	nz,labE56E+2
58696			nop
58697			dec	b
58698			rra
58699			inc	bc
58700			call	nz,32512
58703			nop
58704		labE550:
58704			add	a,b	
58705			nop
58706			jr	nz,labE554
58708		labE554:
58708			ld	(bc),a	
58709			nop
58710		labE556:
58710			ld	bc,0	
58713		labE559:
58713			ld	h,b	
58714			ld	l,c
58715			ld	(61547),hl
58718			ret
58719		labE55F:
58719			ld	a,c	
58720		labE560:
58720			ld	(61549),a	
58723			ret
58724		labE564:
58724			ld	h,b	
58725			ld	l,c
58726			ld	(61552),hl
58729			ret
58730		labE56A:
58730			ld	l,c	
58731			ld	h,b
58732			ret
58733		labE56D:
58733			push	bc	
58734		labE56E:
58734			call	labE642	
58737			pop	bc
58738		labE572:
58738			jp	59145	
58741		labE575:
58741			call	labE642	
58744			ld	a,1
58746			ld	(61545),a
58749			xor	a
58750			ld	(61554),a
58753			jr	labE5C9
58755		labE583:
58755			xor	a	
58756			ld	(61545),a
58759			ld	hl,61554
58762			dec	c
58763			jp	m,labE59A
58766			jr	z,labE596
58768			ld	a,(61546)
58771			ld	(hl),a
58772			jr	labE5C9
58774		labE596:
58774			ld	(hl),128	
58776			jr	labE5C9
58778		labE59A:
58778			or	(hl)	
58779			jr	z,labE5C9
58781			dec	(hl)
58782			ld	hl,(61541)
58785			ex	de,hl
58786			ld	hl,(61543)
58789			or	h
58790			ld	a,40
58792			jp	p,labE5AD
58795			ld	a,64
58797		labE5AD:
58797			inc	l	
58798			sub	l
58799			jr	nz,labE5B3
58801			ld	l,a
58802			inc	de
58803		labE5B3:
58803			push	hl	
58804			ld	hl,(61547)
58807			call	labE63A
58810			pop	de
58811			jr	nz,labE5C5
58813			ld	hl,(61549)
58816			call	labE63A
58819			jr	z,labE5C9
58821		labE5C5:
58821			xor	a	
58822			ld	(61554),a
58825		labE5C9:
58825			ld	hl,(61547)	
58828			ex	de,hl
58829			ld	hl,(61541)
58832			call	labE63A
58835			jr	nz,labE5E3
58837			ld	hl,(61549)
58840			ex	de,hl
58841			ld	hl,(61543)
58844			call	labE63A
58847			and	252
58849			jr	z,labE5F1
58851		labE5E3:
58851			call	labE642	
58854			ld	a,(61554)
58857			and	127
58859			ld	a,0
58861			jr	nz,labE5F0
58863			inc	a
58864		labE5F0:
58864			or	a	
58865		labE5F1:
58865			ld	hl,(61547)	
58868			ld	(61541),hl
58871			ld	hl,(61549)
58874			ld	(61543),hl
58877			ld	b,1
58879			call	nz,labE64C
58882			ld	a,(61543)
58885			and	3
58887			rra
58888			ld	h,a
58889			rra
58890			ld	l,a
58891			ld	de,61696
58894			add	hl,de
58895			ld	de,(61552)
58899			ld	a,(61545)
58902			dec	a
58903			push	af
58904			jr	z,labE61E
58906			ld	(labE6F3),a
58909			ex	de,hl
58910		labE61E:
58910			ld	bc,128	
58913			ldir
58915			pop	af
58916			ret	z
58917			ld	hl,61554
58920			ld	a,(hl)
58921			or	a
58922			jp	p,labE631
58925			xor	a
58926			ld	(hl),a
58927			jr	labE635
58929		labE631:
58929			ld	hl,61555	
58932			inc	(hl)
58933		labE635:
58933			call	z,labE642	
58936			xor	a
58937			ret
58938		labE63A:
58938			ld	a,d	
58939			xor	h
58940			ld	a,255
58942			ret	nz
58943			ld	a,e
58944			xor	l
58945			ret
58946		labE642:
58946			ld	a,(labE6F3)	
58949			or	a
58950			ret	z
58951			xor	a
58952			ld	(labE6F3),a
58955			ld	b,a
58956		labE64C:
58956			ld	a,(61543)	
58959			and	252
58961			rrca
58962			rrca
58963			ld	c,a
58964			ld	d,241
58966			ld	hl,(61541)
58969			ld	a,(61544)
58972			or	a
58973			jp	m,labE039
58976			push	hl
58977			push	bc
58978			call	labE48F
58981			pop	bc
58982			ld	hl,(61544)
58985			ld	h,0
58987			ld	de,labE6F9+1
58990			add	hl,de
58991			ld	e,b
58992			ld	d,c
58993			or	128
58995			ld	c,a
58996			ld	a,(hl)
58997			rra
58998			jr	nc,labE68A
59000			ld	a,e
59001			or	a
59002			jp	z,labE69F
59005			ex	(sp),hl
59006			ld	a,l
59007			cp	35
59009			jr	c,labE686
59011			ld	a,133
59013			sub	l
59014		labE686:
59014			rlca	
59015			rlca
59016			ld	l,a
59017			ex	(sp),hl
59018		labE68A:
59018			xor	a	
59019			ld	a,d
59020			rra
59021			jr	nc,labE691
59023			add	a,5
59025		labE691:
59025			ld	d,a	
59026			ld	a,(hl)
59027			pop	hl
59028			ld	h,1
59030			and	65
59032			jr	nz,labE6A8
59034			ld	a,l
59035			cp	35
59037			jr	c,labE6A8
59039		labE69F:
59039			ld	hl,59884	
59042			call	59927
59045			jp	labE003
59048		labE6A8:
59048			ld	a,h	
59049			ld	b,l
59050			ld	hl,61696
59053			jp	labE033
59056			ld	(49672),sp
59060			ld	sp,49672
59063			push	hl
59064			push	de
59065			push	bc
59066			push	af
59067			halt
59068		labE6BC:
59068			nop	
59069			nop
59070			nop
59071		labE6BF:
59071			call	m,15616	
59074			nop
59075			and	b
59076			and	c
59077			and	d
59078			and	e
59079			ret	m
59080			sub	b
59081			nop
59082			nop
59083			nop
59084			nop
59085			nop
59086			nop
59087			nop
59088			nop
59089			nop
59090			nop
59091			nop
59092			nop
59093			nop
59094			nop
59095			nop
59096			nop
59097			nop
59098			nop
59099			nop
59100			nop
59101			nop
59102			nop
59103			nop
59104			nop
59105		labE6E1:
59105			ld	bc,22784	
59108			ld	e,c
59109			ld	e,c
59110		labE6E6:
59110			jp	po,486	
59113		labE6E9:
59113			ld	a,(bc)	
59114		labE6EA:
59114			ret	
59115			nop
59116			nop
59117			ret
59118			nop
59119			nop
59120			ret
59121			nop
59122			nop
59123		labE6F3:
59123			nop	
59124			nop
59125		labE6F5:
59125			jp	z,62208	
59128		labE6F8:
59128			rst	56	
59129		labE6F9:
59129			ld	bc,0	
59132			nop
59133			nop
59134			db	1
59135		labE6FF:
59135			db	16	
; vim: ts=8 sts=8 sw=8 noet
