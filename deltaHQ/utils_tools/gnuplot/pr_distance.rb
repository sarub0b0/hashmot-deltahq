#!/usr/bin/env ruby
require 'gnuplot'
require 'csv'
require 'pp'
# 座標系の指定
# set ytics nomirror 第一y軸の目盛は左側のみにする
# set y2tics         第二y軸を描画することを指定
# set grid           目盛線を描画することを指定
# set xlabel "Year"  x軸の見出しを指定
# set ylabel "Nikkei average (yen)"           第一y軸の見出しを指定
# set y2label "Land price index (1985 = 100)" 第二y軸の見出しを指定
# 凡例
# set key top left   凡例は左上に描画
# データ描画
# plot 'データファイルＡ' with lines title "Nikkei average", \
#      'データファイルＢ' with lines title "Land price index" axes x1y2
#       "axes x1y2"を指定することでデータファイルＢの目盛は第一x軸と第二y軸にとることを指定

def gnu_plot(gp, data_set, x_key, y1_key, y2_key, xlabel, ylabel, y2label)
  Gnuplot::Plot.new(gp) do | plot|
    # plot.terminal "epslatex color"
    plot.terminal "postscript 20 eps noenhanced color"
    # plot.terminal "pdf 20 noenhanced color"
    # plot.font '"Ricty-Regular.ttf,12"'
    # plot.title title
    #    plot.format 'y "%.2e"'
    plot.ytics 'nomirror'
    plot.y2tics
    plot.xlabel xlabel
    plot.ylabel ylabel
    plot.y2label y2label
    # plot.output "pr_loss_distance.tex"
    plot.output "#{title}.eps"

    # plot.xrange "[0:20]"

    # if y_key == "rate"
    #   plot.yrange "[0:1]"
    # end

    pp x_key, y1_key, y2_key, xlabel, ylabel, y2label
    pp data_set
    #
    i = 0
    for data in data_set do
      if i == 0
        plot.data << Gnuplot::DataSet.new([data[x_key.to_sym],data[y1_key.to_sym]]) do |ds|
          ds.with = 'line'
          ds.linewidth = 1
          ds.title = "received power"
          p i
        end
      else
        # plot.data << Gnuplot::DataSet.new([data[x_key.to_sym],data[y2_key.to_sym]]) do |ds|
        #   ds.with = 'line'
        #   ds.linewidth = 1
        #   ds.title = "loss rate"
        #   p i
        # end
      end
      i = i + 1
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

def csv_to_dataset(csv, type)

  distance = []
  param = []

  csv.each do |data|
    distance << data['distance']
    #time = data['elapsed'].partition(":")
    #sec = time[0].to_f * 60 + time[-1].to_f
    #
    param << data["#{type}"]
  end

  params = {
    type: type,
    distance: distance,
    "#{type}" => param,
  }

  return params

end

#def calc_rate(params_ary)
#  ori = params_ary[0][:elapsed]
#
#  params_ary.each do |params|
#    rate = []
#    params.each_pair do |key, value|
#      if key == :elapsed
#        i = 0
#        value.each do|v|
#          if ori[i] == 0
#            rate << 0
#          else
#            rate << v / ori[i]
#          end
#          i += 1
#        end
#      end
#    end
#    params[:rate] = rate
#  end
#
#  return params_ary
#
#end

if __FILE__ == $0
  # time from_id from_node_x from_node_y from_node_z to_id to_node_x to_node_y to_node_z distance Pr SNR FER num_retr op_rate bandwidth loss_rate delay jitter

  data_set = []
  csv_data = CSV.read("loss_distance.log", headers: true, col_sep: " ")
  data_set << csv_to_dataset(csv_data, "Pr")
  data_set << csv_to_dataset(csv_data, "loss_rate")

  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "distance", "Pr", "loss_rate", "distance[m]", "received power[dBm]", "loss rate[%]")
  end
  #  Gnuplot.open do |gp|
  #    gnu_plot(gp, params_ary, "node_number", "cpu" ,"deltaQ calcuration", "node number", "cpu[%]")
  #  end
  #Gnuplot.open do |gp|
  #  gnu_plot(gp, data_set, "node_number", "rate" , graph_title, "node number", "rate[%]")
  #end
end

