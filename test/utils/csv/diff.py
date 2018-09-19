import csv
import sys

csv.field_size_limit(sys.maxsize)

hcc_result = sys.argv[1]
nvcc_result = sys.argv[2]
reader1 = csv.DictReader(open(hcc_result, 'r'))
amd_op = []
for line1 in reader1:
	amd_op.append(line1)
	
reader2 = csv.DictReader(open(nvcc_result, 'r'))
nv_op = []
for line2 in reader2:
	nv_op.append(line2)

csvfile = open('final_results.csv', 'w')

fieldnames = ['Test name','Performance in Nvidia','Performance in AMD','Input size', 'kernel size', 'output size', 'Results']
writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
writer.writeheader()

def test_compare(amd_op,nv_op):
	time_nv = 0
	flag = 'fail'
	for line1 in amd_op:
		for line2 in nv_op:
			if (line1['Test_name'] == line2['Test_name']):
				flag = 'pass'
				if (line1['Output'] == line2['Output']):
					time_nv = line2['Average Excecution Time (milliseconds)']
					print ("test pass for "+line1['Test_name'])
					print(sum(c1!=c2 for c1,c2 in zip(line1['Output'],line2['Output'])))
					
				else:
					time_nv = line2['Average Excecution Time (milliseconds)']
					flag = "fail"
					print ("test fail for "+line1['Test_name']+"!!!difference in outputs!!!!")
					print(sum(c1!=c2 for c1,c2 in zip(line1['Output'],line2['Output'])))

		writer.writerow({'Test name':line1['Test_name'],'Performance in Nvidia':time_nv,'Performance in AMD':line1['Average Excecution Time (milliseconds)'],'Input size':line1['Input size'], 'kernel size':line1['kernel size'], 'output size':line1['output size'], 'Results':flag})
			
test_compare(amd_op,nv_op)
