#!/usr/bin/env ruby
require 'gnuplot'
require 'csv'
require 'pp'


def gnu_plot(gp, data_set, x_key, y_key,  title, xlabel, ylabel)
  Gnuplot::Plot.new(gp) do | plot|
    plot.title title
    plot.xlabel xlabel
    plot.ylabel ylabel
    plot.termoption "noenhanced"
    #   plot.logscale "x 2"

    for data in data_set do
      plot.data << Gnuplot::DataSet.new([data[x_key.to_sym],data[y_key.to_sym]]) do |ds|
        ds.with = 'line'
        ds.linewidth = 1
        ds.title = "thread #{data[:thread_number]}"
      end
    end

  end
end

def create_plot_params(title, xlabel, ylabel)

  hash = {
    title: title,
    xlabel: xlabel,
    ylabel: ylabel,
  }

  return hash
end

def csv_to_dataset(csv, thread_number)

  data_set = []

  conn_i = []
  user = []
  elapsed = []
  cpu = []

  csv.each do |data|
    conn_i << data['conn_i']
    #time = data['elapsed'].partition(":")
    #sec = time[0].to_f * 60 + time[-1].to_f
    #
    elapsed << data['elapsed'].to_f
  end

  conn_i_tmp = Array.new(conn_i.length)
  elapsed_tmp = Array.new(conn_i.length)
  index = 0

  if thread_number != "0"
    conn_i.length.to_i.times do |i|
      index = conn_i[i].to_i
      conn_i_tmp[index] = conn_i[i]
      elapsed_tmp[index] = elapsed[i]
    end
    params = {
      thread_number: thread_number,
      conn_i: conn_i_tmp,
      elapsed: elapsed_tmp,
    }
  else
    params = {
      thread_number: thread_number,
      conn_i: conn_i,
      elapsed: elapsed,
    }
  end


  return params

end

def calc_rate(params_ary)
  ori = params_ary[0][:elapsed]

  params_ary.each do |params|
    rate = []
    params.each_pair do |key, value|
      if key == :elapsed
        i = 0
        value.each do|v|
          if ori[i] == 0
            rate << 0
          else
            rate << v / ori[i]
          end
          i += 1
        end
      end
    end
    params[:rate] = rate
  end

  return params_ary

end

if __FILE__ == $0
  # time from_id from_node_x from_node_y from_node_z to_id to_node_x to_node_y to_node_z distance Pr SNR FER num_retr op_rate bandwidth loss_rate delay jitter

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
      params_ary << csv_to_dataset(csv_data, i)
    end
    j = j + 1
  end

  data_set = []
  data_set = calc_rate(params_ary)


  #  Gnuplot.open do |gp|
  #    gnu_plot(gp, params_ary, "node_number", "user" ,"deltaQ calcuration", "node number", "time[s]")
  #  end
  Gnuplot.open do |gp|
    gnu_plot(gp, params_ary, "conn_i", "elapsed", dir, "node conn_i", "elapsed[s]")
  end
  #  Gnuplot.open do |gp|
  #    gnu_plot(gp, params_ary, "node_number", "cpu" ,"deltaQ calcuration", "node number", "cpu[%]")
  #  end
  #Gnuplot.open do |gp|
  #  gnu_plot(gp, data_set, "node_number", "rate" ,"deltaQ calcuration", "node number", "rate[%]")
  #end
end

