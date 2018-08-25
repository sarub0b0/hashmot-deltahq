require 'gnuplot'
require 'csv'
require 'pp'


def gnu_plot(gp, data_set, x_key, y_key,  title, xlabel, ylabel)
  Gnuplot::Plot.new(gp) do | plot|
    plot.title title
    plot.xlabel xlabel
    plot.ylabel ylabel

    for data in data_set do
      plot.data << Gnuplot::DataSet.new([data[x_key.to_sym],data[y_key.to_sym]]) do |ds|
        ds.with = 'line'
        ds.linewidth = 1
        ds.title = "#{data[:from_id]} -> #{data[:to_id]}"
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

def csv_to_dataset(csv, node_number)


  data_set = []

  node_number.times do |i|
    node_number.times do |j|

      d = []
      time = []
      pr = []
      snr = []
      fer = []
      num_retr = []
      op_rate = []
      bandwidth = []
      loss_rate = []
      delay = []
      jitter = []

      if i != j
        csv.each do |data|
          if data['from_id'] == i.to_s && data['to_id'] == j.to_s
            d << data['distance']
            time << data['time']
            pr << data['Pr']
            snr << data['SNR']
            fer << data['FER']
            num_retr << data['num_retr']
            op_rate << data['op_rate']
            bandwidth << data['bandwidth']
            loss_rate << data['loss_rate']
            delay << data['delay']
            jitter << data['jitter']
          end
        end

        if [d, time, pr, snr, fer, num_retr, op_rate, bandwidth, loss_rate, delay, jitter].any? {|a| a.nil?}
          raise StandardError.new("nil arrray")
        end

        params = {
          from_id: i,
          to_id: j,
          time: time,
          distance: d,
          Pr: pr,
          SNR: snr,
          FER: fer,
          num_retr: num_retr,
          op_rate: op_rate,
          bandwidth: bandwidth,
          loss_rate: loss_rate,
          delay: delay,
          jitter: jitter,
        }

        data_set << params

      end
    end
  end

  return data_set

end

if __FILE__ == $0
  csv_data = CSV.read(ARGV[0], headers: true, col_sep: " ")
  # time from_id from_node_x from_node_y from_node_z to_id to_node_x to_node_y to_node_z distance Pr SNR FER num_retr op_rate bandwidth loss_rate delay jitter

  node_number = ARGV[1].to_i

  data_set = csv_to_dataset(csv_data, node_number)
=begin
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "distance", "Pr" ,"distance-Pr", "distance[m]", "Pr[dB]")
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "distance", "bandwidth", "distance-bandwidth", "d[m]", "bandwidth[Mb/s]" )
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "distance", "delay", "distance-delay", "d[m]", "delay[ms]" )
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "distance", "jitter", "distance-jitter", "d[m]", "jitter[ms]" )
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "distance", "FER", "distance-FER", "d[m]", "FER[]" )
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "distance", "loss_rate", "distance-loss rate", "d[m]", "loss rate[]" )
  end
=end


  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "time", "distance" ,"time-distance", "time[s]", "distance[m]")
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "time", "Pr" ,"time-Pr", "time[s]", "Pr[dB]")
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "time", "bandwidth", "time-bandwidth", "time[s]", "bandwidth[Mb/s]" )
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "time", "delay", "time-delay", "time[s]", "delay[ms]" )
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "time", "jitter", "time-jitter", "time[s]", "jitter[ms]" )
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "time", "FER", "time-FER", "time[s]", "FER[]" )
  end
  Gnuplot.open do |gp|
    gnu_plot(gp, data_set, "time", "loss_rate", "time-loss rate", "time[s]", "loss rate[]" )
  end
end

