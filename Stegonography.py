#!/usr/bin/python
#Introduction to Digital Forensics Project

"""				UPDATES / CHANGE LOG
	11/28/13 - Script is fully functional
	11/29/13 - Now supports zipping files!
	12/1/13 - Now zips files by default
	12/2/13 - Now extracts data more efficiently
				(Stops looking through pixels once
				we've got the bits we need)
"""
 
#Python libraries needed for most operations
import sys, PIL, getopt, struct, os
#Import image module for loading/manipulating images
from PIL import Image
#Import library for writing to zip files
from zipfile import ZipFile

#Writes a list of bytes to the provided file
def bytesToFile(payload, filename):
	try: #In case file cannot be opened
		#Opens the file
		f = open(filename, 'wb')
		
		#Goes through each byte and writes to the file
		for byte in payload:
			f.write(struct.pack('c',byte))
		
		#Closes the file stream
		f.close()
	except:#If file could not be opened, print error and exit
		print "Error: Could not open %s for writing!" % filename
		sys.exit()
	
	#prints success message
	if filename[-4:] == '.zip':
		print "Recovered files are located in /recovered/ directory"
	else:
		print "Successfully recovered '%s'" % filename

#Takes a string of bit values and returns a list of byte values
def bitsToFile(bits):
	bytes = []
	
	#Go through each 8-bit chunk and turn it into a byte!
	for i in range(0, len(bits), 8):
		bytes.append(bitsToByte(bits[i:i+8]))
	
	#Return our list of bytes
	return bytes

#Takes a string of bits and turns it into a byte
def bitsToByte(bits):
	#Checks to see if there are enough bits
	if len(bits)%8 != 0:#Prints error if not, then exits
		print "Error: There was an incorrect number of bits read"
		sys.exit()
	else:#Otherwise returns byte
		return chr(int(bits,2))

#This function is in case we think there may not be hidden
#files in a picture that a user wants us to look in
def keepLooking():
	#Tell user we don't think anything is here, and ask if
	#we should continue
	choice = raw_input(
		"Nothing likely to be here... continue [Y/N]? ")
	#If user says no, then we exit
	if choice.lower() == 'n': sys.exit()

#When passed an image, looks through pixels and
#Tries to find hidden files
def getFileBits(image):
	#Default values for our variables
	bits, zipped, numFiles, fileSizes = "", False, 0, []
	pix, totalSize = image.load(), 0
	availableSpace = (image.size[0] * image.size[1] * 3.0)/8.0
	
	#Go through each pixel
	for x in range(image.size[0]):
		for y in range(image.size[1]):
			#Goes through each value in pixel
			for value in pix[x,y]:
				#If value is even, then bit is 0
				if value % 2 == 0: bits += '0'
				#If value is odd, then bit is '1'
				else: bits += '1'
				
				#If we've read in the boolean and number of files
				if len(bits) == 33 and numFiles == 0:
					#Set boolean accordingly
					if bits[0] == '1': zipped = True
					#Set number of files
					numFiles = int(bits[1:],2)
					
					#Maybe nothing is here... ask user to continue
					if numFiles < 0: keepLooking()
					elif (33+32*numFiles+8*numFiles) > availableSpace: keepLooking()
					
					#Reset bits
					bits = ""
				#If we've read in the file sizes
				elif len(bits)==(numFiles*32) and len(fileSizes)==0:
					#For each integer
					for i in range(numFiles):
						#Convert from binary
						size = int(bits[:32],2)
						#Add file size to our list
						fileSizes.append(size)
						#Remove from binary string
						bits = bits[32:]
						#Keep track of total size
						totalSize += size
						
						#Maybe nothing is here... ask user to continue
						if (totalSize*8) > (availableSpace-(33+numFiles*32)): keepLooking()
						
					#Reset bits once we're done
					bits = ""
				#If we've read all that we NEED to, then return info
				elif len(bits)==(totalSize*8) and len(fileSizes)>0:
					return numFiles, zipped, fileSizes, bits

#When passed the steg file and requested payload name,
#opens image and looks for hidden files
def revealFiles(steg, payload):
	#Set all of our variables to the output of getFileBits
	numFiles, zipped, fileSizes, bits = getFileBits(
										openImage(steg))
	
	#Get our bits to files!
	for i in range(numFiles):
		#Temp payload name variable
		payloadName = payload
		#If this is a zip file, add extension
		if zipped: payloadName += '.zip'
		#Otherwise add a number to the end
		else: payloadName += ('%s' % (i+1))
		#Get the file from the bits
		bytesToFile(bitsToFile(bits[:fileSizes[i]*8]), payloadName)
		#Get rid of bits we just used
		bits = bits[fileSizes[i]*8:]
	
	#If recovered file is zipped
	if zipped:
		#Extract all data to /recovered/ directory
		with ZipFile(payload+'.zip', 'r') as myzip:
			myzip.extractall('recovered/')
		#Remove zip file
		os.remove(payload+'.zip')
	

#When passed all bits in a list, the image object, and the requested file
#name to save to, this function whips it all together and saves the image.
def saveImage(bits, image, steg):
	#Used as a counter as we pass through each pixel
	bit = 0
	#Loads pixels into 2D list
	pixels = image.load()
	
	#Goes through each pixel value and inserts our bits into them
	for i in range(image.size[0]):
		for j in range(image.size[1]):
			RGB = []
			for value in pixels[i,j]:
				if bit < len(bits):
					RGB.append(putBitInNumber(int(bits[bit]),value))
					bit += 1
				else:
					if len(RGB) < 3:
						RGB.append(value)
			if bit >= len(bits): break
			pixels[i,j] = (RGB[0],RGB[1],RGB[2])
	image.save(steg)

#This function is passed a bit and a color value, then inserts the bit
def putBitInNumber(bit, number):
	#If bit is 1
	if bit:
		#If the number already contains bit,
		if (number & bit): return number	#Return the number
		#Otherwise, return number + 1
		else: return (number+1)
	#If the bit is 0
	elif not bit:
		#And number doesn't have the bit
		if not (number & 1): return number	#Return the number
		#Otherwise, return number - 1
		else: return (number-1)

#When passed a list of file paths, puts all files into zip
#file and returns a list containing zip file path
def zipFiles(payloads):
	with ZipFile('zip.zip', 'w') as myzip:
		for payload in payloads:
			myzip.write(payload, os.path.basename(payload))
	return ['zip.zip']
		
#This function controls the hiding of payloads within a file
def hideFiles(cover, payloads, steg, zipped):
	if zipped: payloads = zipFiles(payloads)
	
	#Opens the image and stores to im variable
	im = openImage(cover)
	
	#Set available space that we have from cover file
	availableSpace = (im.size[0]*im.size[1]*3)
	
	numFiles = 0
	fileSizes = []
	
	#Set total size required by cover file for payloads
	totalReqSize = 4
	for payload in payloads:
		numFiles += 1	#Add to the number of files
		size = os.path.getsize(os.path.realpath(payload))
		#Add the size of the file, as well as 4 bytes for int
		totalReqSize += (size + 4)
		#Add file sizes to list
		fileSizes.append(size)
	totalReqSize += (numFiles*4)
	
	#Convert to size in bits
	totalReqSize *= 8
	
	#Check sizes and space availability
	if  availableSpace < totalReqSize:#If there is not enough room
		#Print error message
		print "Error: %s does not meet required size to host payloads" % cover
		#Remove temporary zip file if there is one
		if zipped: os.remove(payloads[0])
		sys.exit() #Then exit
	else:#If there is enough room
		#Give user some hope
		print "Hiding files..."
		#Empty list to store bits
		bits = []
		#Set zipped boolean to True if flag is given
		if zipped: bits += '1'
		else: bits += '0'
		#Add bits of numFiles
		bits += numToBits(numFiles)
		#Add bits for the file sizes
		for i in range(len(fileSizes)):
			bits += numToBits(fileSizes[i])
		#Add bits for the files
		for payload in payloads:
			bits += fileToBits(payload)
		#Remove temporary zip file if there is one
		if zipped: os.remove(payloads[0])
		#Save the image and cross our fingers!
		saveImage(bits, im, steg)
		print "Hidden files in %s!" % steg	

#Prints helpful information if user requests
def help():
	print "\n'%s' is a program that help keep files hidden from nosy neighbors." % sys.argv[0]
	print "It can be used by either running the program without any arguments,"
	print "in which case it will prompt for proper input, or with the"
	print "following command line arguments:\n"
	print "-h OR --hide"
	print "	Sets mode to 'hide' and is used when attempting to hide a"
	print "	file or files within an image."
	print "-r OR --reveal"
	print "	Sets mode to 'reveal' and is used when attempting to recover"
	print "	hidden files from an image."
	print "-i"
	print "	Gives an input file, the first of which MUST be the cover"
	print "	image if mode is set to 'hide'. Any following inputs must"
	print "	have another '-i' and will be considered payloads to hide."
	print "	When using in 'reveal' mode, only the first given will be"
	print "	considered when looking for hidden files."
	print "-o"
	print "	Give the desired output file name. Only the first given"
	print "	will be considered."
	print "-"
	print "	Does not zip files prior to hiding them within an image."
	print "	It is recommended to not use this option, for file names"
	print "	and their types are not stored unless they are compressed."
	print "--help OR -?"
	print "	Prints this message."
	print "\nExample usage:"
	print "	For hiding files:"
	print "	  %s --hide -i <cover file> -i <payload> -o <stegfile>" % sys.argv[0]
	print "	For recovering files:"
	print "	  %s --reveal -i <stegfile> -o <payload>" % sys.argv[0]

#When passed a filename, convert to list of bits from the file
def fileToBits(filename):
	#Empty list to store the bits
	bits = []
	#Empty string to store raw bytes from file
	bytes = ""
	
	try:
		#Read in file to bytes variable
		with open(filename, 'rb') as f:
			bytes = f.read()
			f.close()
	except:
		print "Could not open %s for reading!" % filename
		sys.exit()
	
	#Go through each byte and convert it into bits
	for byte in bytes:
		bits += byteToBits(struct.unpack('c', str(byte))[0])
	
	#Return a list of each bit of the file
	return bits

#When passed a byte from a file, convert to list of bits
def byteToBits(byte):
	#Empty list to start the bits
	bits = []
	
	#For each bit in the byte
	for bit in range(8):
		#Add it to the list
		bits.append((( ord(byte) >> (7-bit) ) & 1))
	
	#Return list of bits from byte
	return bits

#This function, when passed a number, returns a list of bit values
def numToBits(num):
	bits = ""
	for bit in list('{0:32b}'.format(num)):
		bits += bit
	
	bits = list(bits)
	
	for i in range(len(bits)):
		if bits[i] != '1' and bits[i] != '0': bits[i] = '0'
	
	return bits

#Function for opening image object.
def openImage(filename):
	try:	#Attempt to open as image
		#If it works, assign to im var
		return Image.open(filename)
	except:	#If there are any errors, then give error message and exit
		print "%s is either not an image, or is not supported by this program." % filename
		sys.exit()

"""
This function is strictly just to parse command line arguments
It is not called unless the program is called with any kind of argument,
and returns values (assuming inputting correctly) to the main function
for further processing.

********************************NOTE************************************
*When using command line, it is important to set the mode switch first.*
*If the mode is to hide a file, the first input file given must be the *
*cover image. If there are more than one input/output files, user must *
*use the corresponding switch per file. If these quidelines are not    *
*followed, then undesired effects may occur.                           *
************************************************************************
"""
def parseArgs(argv):
	#************DEFAULT vars******************
	hide, reveal = False, False				#Mode var
	inputfiles = []							#List for input files
	steg = ''								#Default stegfile filename
	cover = ''								#Default cover image filename
	payloads = []							#List for storing any payloads
	output = ''								#Default output file filename
	zipped = True							#Default zip status
	#***********************************************
	
	#Attempts to parse arguments using the getopt library
	try:	#If arguments are valid, then store them to lists titled opts and args
			opts, args = getopt.getopt(argv, "hrni:o:", ["help", "hide", "reveal", "nozip"])
	except getopt.GetoptError:	#If getopt function raises exception, then print error message
			print '%s <-h/r mode> <-i input file> <-o output file>' % sys.argv[0]
			sys.exit(2)			#Then exit
	
	#Go through each given argument and 
	for opt, arg in opts:
			if opt.lower() in ('--help', '-?'): #If user requests help, give a little advice
				help()
				sys.exit()					#Then exit
			elif opt.lower() in ('-h', '--hide'):#If user sets mode to hide
				hide = True					#Make hide var true
			elif opt.lower() in ('-r', '--reveal'):	#If user sets mode to reveal
				reveal = True				#Set reveal var to true
			elif opt.lower() in ('-i',):	#If input file is given
				inputfiles.append(arg)		#Add to input file list
			elif opt.lower() in ('-o',):	#If output file is given
				output = arg				#Set output file var
			elif opt.lower() in ('-n', '--nozip'):#Do not zip files
				zipped = False
	
	#If there are no input files, give error message
	if len(inputfiles) < 1:
			print "%s requires at least one input file." % sys.argv[0]
			sys.exit()				#Then exit
	elif hide and not reveal:		#If hide var is true
			cover = inputfiles[0]	#Then first input file is cover file
			#Steg file will be set output file
			if output[-4:] != '.png':
				steg = (output+'.png')
			else: steg = output
	elif reveal and not hide:		#If reveal var is true
			steg = inputfiles[0]	#Steg file will be first input
			payloads.append(output)	#Payload will be put in output file
	else:							#If neither mode is set, give error message
		print "ERROR: %s requires a single mode to work in." % sys.argv[0]
		print "Example: '%s --hide -i <cover file> -i <payload> -o <stegfile>'" % sys.argv[0]
		print "OR '%s --reveal -i <stegfile> -o <payload>'" % sys.argv[0]
		sys.exit()					#Then exit
	
	#If there is more than one input file, and mode is not reveal
	if len(inputfiles) > 1 and not reveal:
			for item in inputfiles[1:]:
					payloads.append(item)	#Add them to payload file list
 
	#Return all processed vars as a tuple
	return (hide, reveal, steg, cover, payloads, zipped)

#Traverses through string, separating it into smaller
#strings with a delimiter of ','
def parseInput(filenames):
	#Default vars
	payloads = []
	temp = ''
	
	#Go through each letter in filenames var
	for letter in filenames:
		if letter != ',':						#If letter is not delimiter
			if letter != ' ': temp += letter	#If letter is not space, add to temp var
			elif temp != '': temp += letter		#If letter is space, but temp var is not empty, then add
		else:									#If letter is delimiter
			payloads.append(temp)				#Add temp var to payloads
			temp = ''							#Reset temp var
	
	payloads.append(temp)						#Adds the last temp var remaining to payloads
			
	return payloads								#Return list of filenames

#This function prompts user for all vars necessary
def promptUser():
	#************DEFAULT VARS******************
	choice = ''
	hide, reveal, zipped = False, False, True
	cover = ''
	steg = ''
	payloads = []
	#***********************************************
	
	#Keep prompting user until we receive valid mode
	while choice != 'r' and choice != 'h':
		choice = raw_input("What mode would you like to run in [H/R]? ")
		if choice.lower() == 'h': hide = True
		elif choice.lower() == 'r': reveal = True
		else: print "That was not an available option."
	
	#If user sets mode to hide
	if hide:
		#Get necessary information
		cover = raw_input("Cover Image File Path: ")
		temp = raw_input("Payload file paths (separate multiple file names with commas): ")
		payloads = parseInput(temp) #Parse the input for multiple filenames
		steg = raw_input("Desired stegfile File Name: ")
		if steg[-4:] != '.png': steg += '.png'
		zip = raw_input("Is it okay to zip the files [Y/N]? ")
		if zip.lower() == 'n': zipped = False
	#If user sets mode to reveal
	else:
		#Get necessary information
		steg = raw_input("Stegfile File Path: ")
		payloads.append(raw_input("Desired payload file name: "))
	
	#Return received information
	return (hide, reveal, steg, cover, payloads, zipped)
	
"""
Main function, called from if statement below.
Passed any and all command-line arguments that
were given.
"""
def main(argv):
	#************DEFAULT vars******************
	hide, reveal, zipped = False, False, True
	steg = ''
	cover = ''
	payloads = []
	#***********************************************
    
	#If there were any command line arguments given
	if len(argv) > 0:
		#Then parse them and initialize vars
		hide, reveal, steg, cover, payloads, zipped = parseArgs(argv)
	#Otherwise, prompt user for necessary information
 	else:
		hide, reveal, steg, cover, payloads, zipped = promptUser()
	
	#If hide mode is set, then hide all the files!
 	if hide: hideFiles(cover, payloads, steg, zipped)
 	#If reveal mode is set, reveal all the files!
	if reveal: revealFiles(steg, payloads[0])
	
#If this is the main script, then call main function with command line args
if __name__ == '__main__':
        main(sys.argv[1:])
