import sys
import os.path as path
import math

class LogStatistics (object):
	def __init__(self) :
		self.nEntries = 0
		self.totalXDeviation = 0
		self.totalYDeviation = 0
		self.totalDDeviation = 0
		self.totalDistanceDeviation = 0
		self.maxXDeviation = -1
		self.maxXDeviationLineNumber = -1
		self.maxYDeviation = -1
		self.maxYDeviationLineNumber = -1
		self.maxDDeviation = -1
		self.maxDDeviationLineNumber = -1
		self.maxDistanceDeviation = -1
		self.maxDistanceDeviationLineNumber = -1
		

	def doLine(self, line, lineNumber) :
		try:
			tokens = line.split()

			worldPos = 9
			xipPos = 16
			xDeviation = math.fabs(float(tokens[worldPos]) - float(tokens[xipPos]))

			worldPos += 2
			xipPos += 2
			yDeviation = math.fabs(float(tokens[worldPos]) - float(tokens[xipPos]))

			worldPos += 2
			xipPos += 2
			dDeviation = math.fabs(float(tokens[worldPos]) - float(tokens[xipPos][:-1]))

			distanceDeviation = math.sqrt(xDeviation**2 + yDeviation**2)

		except:
			return
		
		
		self.nEntries += 1
		self.totalXDeviation += xDeviation
		self.totalYDeviation += yDeviation
		self.totalDDeviation += dDeviation
		self.totalDistanceDeviation += distanceDeviation

		if self.maxXDeviation < xDeviation :
			self.maxXDeviation = xDeviation
			self.maxXDeviationLineNumber = lineNumber
			
		if self.maxYDeviation < yDeviation :
			self.maxYDeviation = yDeviation
			self.maxYDeviationLineNumber = lineNumber
			
		if self.maxDDeviation < dDeviation :
			self.maxDDeviation = dDeviation
			self.maxDDeviationLineNumber = lineNumber

		if self.maxDistanceDeviation < distanceDeviation :
			self.maxDistanceDeviation = distanceDeviation
			self.maxDistanceDeviationLineNumber = lineNumber


	def __str__(self) :
		if not self.nEntries :
			return ""
		msg = ""
		msg += "X average deviation : " + str(self.totalXDeviation / self.nEntries) + "\n"
		msg += "Maximum X deviation : " + str(self.maxXDeviation) + " in line " + str(self.maxXDeviationLineNumber) +"\n"
		msg += "Y average deviation : " + str(self.totalYDeviation / self.nEntries) + "\n"
		msg += "Maximum Y deviation : " + str(self.maxYDeviation) + " in line " + str(self.maxYDeviationLineNumber) +"\n"
		msg += "D average deviation : " + str(self.totalDDeviation / self.nEntries) + "\n"
		msg += "Maximum D deviation : " + str(self.maxDDeviation) + " in line " + str(self.maxDDeviationLineNumber) +"\n"
		msg += "Distance average deviation : " + str(self.totalDistanceDeviation / self.nEntries) + "\n"
		msg += "Maximum distance deviation : " + str(self.maxDistanceDeviation) + " in line " + str(self.maxDistanceDeviationLineNumber) +"\n"
		return msg



def main() :
	
	if len(sys.argv) != 2 :
		print "Usage: python ", sys.argv[0], " logFile\n"
		return

	if not path.exists(sys.argv[1]) :
		print "File", sys.argv[1], "doesn't exist"
		return

	file = open(sys.argv[1], "r")
	stats = LogStatistics()

	ecx = 1
	for line in file:
		stats.doLine(line, ecx)
		ecx += 1

	file.close()

	print stats



if __name__ == "__main__" :
	main()
