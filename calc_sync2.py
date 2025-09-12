
def calc_sync2 (b, d, l):
	a = 34 - b
	if a < 0:
		a += 35
		b = a
		l |= 0x40
	a = l
	a &= 0x40
	a |= b
	a = ((a & 0x01) << 7) | ((a & 0xfe) >> 1)
	a = ((a & 0x01) << 7) | ((a & 0xfe) >> 1)
	a &= 0xf0
	a += d
	return a

print ("%02x" % calc_sync2 (0x3d, 0x01, 0x81))

