#!/usr/bin/env ruby
# Usage: q-generate.rb template datafile speed

text = IO.readlines(ARGV[1])
title = text.shift.chomp
lines = text.length
text = text.reduce(:+)
chars = text.length - lines * 3
limit = (chars.to_f * 60.0 / ARGV[2].to_f).ceil

IO.foreach(ARGV[0]) do |line|
  print line.sub(/%title/, title).sub(/%text\n/, text).sub(/%chars/, chars.to_s).sub(/%lines/, lines.to_s).sub(/%limit/, limit.to_s).sub(/%speed/, ARGV[2])
end
