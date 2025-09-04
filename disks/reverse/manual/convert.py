
import sys
import re

fn = sys.argv[1]
with open(fn, 'r') as f:
	lines = f.readlines()

for l in lines:
	regex = r'(?<!\w)\d\d+(?!\w)'

	nums = re.findall(regex, l)
	hexs = [ '0%XH' % int(n) for n in nums ]

	txt  = re.split(regex, l)
	
	print(txt[0], end='')

	if len(hexs) >= 1:
		print(hexs[0], end='')
		print(txt[1],  end='')

	if len(hexs) >= 2:
		print(hexs[1], end='')
		print(txt[2],  end='')

	if len(hexs) >= 3:
		print(hexs[2], end='')
		print(txt[3],  end='')

	if len(hexs) >= 4:
		print(hexs[3], end='')
		print(txt[4],  end='')

# end of file
