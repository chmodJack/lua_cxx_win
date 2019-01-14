str="I am so cool"
tbl={name="shun",id=20114442}

function add(a,b)
	return a + b
end

avg,sum=cxx_func(1,3,5,7)

print("avg: ",avg)
print("sum: ",sum)

cxx_print_num(avg,sum)

function all()
	cxx_print_num(avg+1,sum+1)
end
