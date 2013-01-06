###############################################################################
# Emerald - code test
# Sum two values and print the result
# Language's tested features: output, methods.
###############################################################################

def f1( arg1, arg2, arg3 )
	puts( "vector (MET_ARG): #{arg1} + #{arg2} = #{arg3}\n" )
end

f1( 1, 2, 3 )

def f2()
	a = 1
	b = 2
	c = a + b
	puts( "suma (MET_LOCAL): #{a} + #{b} = #{c}\n" )
end

f2()

a1 = 25
a2 = 50
a3 = a1 + a2

puts( "suma (MAIN): #{a1} + #{a2} = #{a3}\n" )

b1 = 1
b2 = 'a'
b3 = 'b'
b4 = 5.25
b5 = 2

puts( "vector (MAIN): #{b1}, #{b2}, #{b3}, #{b4}, #{b5}\n" )

#f1( a1, a2, a3 )
#f2()

#def f3( arg1, arg2, arg3 )
#	puts( "#{arg1} + #{arg2} = #{arg3}\n" )
#end

#b1 = 'a'
#b2 = 1
#b3 = 'b'

#puts( "#{b1} + #{b2} = #{b3}\n" )
#f3( b1, b2, b3 )



#def foo1(a, b, c, d, e, f, g, h)
#	puts("array: #{a} #{b} #{c} #{d} #{e} #{f} #{g} #{h}\n")
#end

#def foo2(a, b, c, d, e, f, g, h)
#	puts("array: #{a} #{b} #{c} #{d} #{e} #{f} #{g} #{h}\n")
#end

#foo1(1,3,5,7,2,4,6,8)
#foo2(1,'a',2,'b',3,'c',4,'d')

#def foo3 ( a, b )
#	a = 5
#	puts( "#{a} #{b}\n" )
#end

#foo3( 6, 6 )

# Sum method definition.
#def sum( a, b)
#	while ( b > 0 ) do
#		a = a + 1
#		b = b - 1
#	end
	#puts( "#{b}\n" );
#	puts( "[#{c}]: #{a} + #{b} = #{result}\n" )
#	result = a
#end 

# Sum method call.
#a = 5
#b = 7
#result = sum( a, b )

#c = 'a'

# Print the result.
#puts( "[#{c}]: #{a} + #{b} = #{result}\n" )


#g1 = 3.5
#g2 = 4.5
#g3 = g1 + g2

#g1 = getf
#puts( "g1 = #{g1}\n" );
#puts( "g1 = #{g1} + #{g2} = #{g3}\n" );
