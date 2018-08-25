#!/usr/bin/env ruby
require 'gnuplot'
require 'csv'
require 'pp'


def gnu_plot(gp, data_set, x_key, y_key, title, xlabel, ylabel)
  Gnuplot::Plot.new(gp) do | plot|
    # plot.terminal "epslatex color"
    plot.terminal "postscript 20 eps noenhanced color"
    # plot.terminal "pdf 20 noenhanced color"
    # plot.font '"Ricty-Regular.ttf,12"'
    # plot.title title
#    plot.format 'y "%.2e"'
    plot.xlabel xlabel
    plot.ylabel ylabel
    # plot.output "#{title}.tex"
    plot.output "#{title}.eps"

    # plot.yrange "[0:3]"
    plot.xrange "[0:1024]"

    # if y_key == "rate"
    #   plot.yrange "[0:1]"
    # end

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

def csv_to_dataset(csv, thread_number, func)

  data_set = []

  node_number = []
  user = []
  elapsed = []
  cpu = []

  csv.each do |data|
    node_number << data['node_number']
    #time = data['elapsed'].partition(":")
    #sec = time[0].to_f * 60 + time[-1].to_f
    #
    elapsed << data["#{func}"].to_f
  end

  params = {
    thread_number: thread_number,
    node_number: node_number,
    elapsed: elapsed,
  }


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

  dir = File.expand_path(ARGV[0])
  csv_data = CSV.read("#{dir}/#{ARGV[1]}_#{ARGV[2]}.log", headers: true, col_sep: " ")
  params_ary << csv_to_dataset(csv_data, ARGV[1], "theoretical")
  params_ary << csv_to_dataset(csv_data, ARGV[2], "real")

  pp params_ary

  data_set = []

  title = "#{ARGV[1]}_#{ARGV[2]}"
  #data_set = calc_rate(params_ary)


  #  Gnuplot.open do |gp|
  #    gnu_plot(gp, params_ary, "node_number", "user" ,"deltaQ calcuration", "node number", "time[s]")
  #  end
  Gnuplot.open do |gp|
    gnu_plot(gp, params_ary, "node_number", "elapsed", title, "number of node", "elapsed[s]")
  end
  #  Gnuplot.open do |gp|
  #    gnu_plot(gp, params_ary, "node_number", "cpu" ,"deltaQ calcuration", "node number", "cpu[%]")
  #  end
  #Gnuplot.open do |gp|
  #  gnu_plot(gp, data_set, "node_number", "rate" , graph_title, "node number", "rate[%]")
  #end
end

