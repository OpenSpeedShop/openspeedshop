      program perf
      integer i
      character c
      integer work 
      integer size 

      size = 100
      n=work(size)
100   continue
      end

      integer function work( n )
      integer n
      integer f1, f2, f3
      integer j
      integer t = 0

      do 01 i = 1, n
      do 01 j = 1, n
01       t = t + f3(i, j) - f2(i, j) - f1(i,j)
      work = t
      end

      integer function f3( x, y)
      integer x, y
      integer t = 0

      do 30 i = 1, 15000
30        t = t + x * y
      f3 = t
      end 

      integer function f2 ( x, y)
      integer x, y
      integer t = 0

      do 20 i = 1, 10000
20       t = t + x * y
      f2 = t
      end


      integer function f1 (x, y)
      integer x, y
      integer t = 0

      do 10 i = 1, 5000
10       t = t + x * y
      f1 = t
      end
