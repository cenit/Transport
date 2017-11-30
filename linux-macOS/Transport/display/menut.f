      program test
c
      character*30 menut(8)
c
      menut(1) = '123456789012345678901234567890'
      menut(2) = 'abcdefghijklmnopqrstuvwxyz    '
      menut(3) = 'b                            y'
      menut(4) = '1                            2'
      menut(5) = '3                            4'
      menut(6) = '5                            6'
      menut(7) = '7                            8'
      menut(8) = 'a                            z'
c
      nx = 30
      ny = 8
      kx = 10
      nr = menu(x1,y1,nx,ny,menut,kx)
      write(6,*) nr
c
      end

