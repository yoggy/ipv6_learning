#!/usr/bin/ruby

require 'socket'

s = TCPServer.open("::", 12345)
loop do
  c = s.accept
  str = c.gets
  puts "recv: #{str}"
  c.puts str
  c.close
end
