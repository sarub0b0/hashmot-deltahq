#!/usr/bin/env ruby
require 'csv'
require 'pp'
require 'numo/gnuplot'

def csv_to_dataset(csv_data)

  elapsed = []

  csv_data.each do |data|
    elapsed << data['elapsed'].to_f
  end

  return elapsed
end

if __FILE__ == $0

  if ARGV.length == 0
    puts "Set Argument thread number"
    exit(1)
  end

  j = 0;
  params_ary = []
  dir = ""
  ARGV.each do |i|
    if j == 0
      dir = File.expand_path(i)
    else
      csv_data = CSV.read("#{dir}/thread_#{i}.log", headers: true, col_sep: " ")
      params_ary << csv_to_dataset(csv_data)
    end
    j = j + 1
  end


  num = 999000
  a = params_ary[0]
  b = params_ary[1]
  #a = Array.new(num)
  #b = Array.new(num)
  #num.times do |i|
  #  a[i] = params_ary[0][i]
  #  b[i] = params_ary[1][i]
  #end

  Numo.gnuplot do
    plot [a, u: '(0):1', w: :boxplot],
         [b, u: '(1):1', w: :boxplot]
  end

end

