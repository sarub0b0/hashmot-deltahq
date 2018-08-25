require 'gnuplot'
require 'csv'
require 'pp'


def gnu_plot(gp, data)
  Gnuplot::Plot.new(gp) do | plot|
    plot.multiplot
    plot.title data[:title]
    plot.xlabel data[:xlabel]
    plot.ylabel data[:ylabel]

    plot.data << Gnuplot::DataSet.new([data[:x],data[:y]]) do |ds|
      ds.with = 'line'
      ds.linewidth = 1
      ds.notitle
    end

  end
end

def create_data_set(title, xlabel, ylabel, x, y)

  hash = {
    title: title,
    xlabel: xlabel,
    ylabel: ylabel,
    x: x,
    y: y,
  }

  return hash
end

def csv_to_hash(csv)

  d = []
  pr = []
  snr = []
  fer = []
  num_retr = []
  op_rate = []
  bandwidth = []
  loss_rate = []
  delay = []
  jitter = []

  hash = {}
  csv.each do |data|
    if data['from_id'] == ARGV[1] && data['to_id'] == ARGV[2]
      d << data['distance']
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

  hash = {
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

  return hash
end

if __FILE__ == $0
  csv_data = CSV.read(ARGV[0], headers: true, col_sep: " ")
  # time from_id from_node_x from_node_y from_node_z to_id to_node_x to_node_y to_node_z distance Pr SNR FER num_retr op_rate bandwidth loss_rate delay jitter


  hash = csv_to_hash(csv_data)

  Gnuplot.open do |gp|
    ds = create_data_set('distance-Pr', 'd[m]', 'Pr[dB]', hash[:distance], hash[:Pr])
    gnu_plot(gp, ds)
  end
  Gnuplot.open do |gp|
    ds = create_data_set('d-bandwidth', 'd[m]', 'bandwidth[Mb/s]', hash[:distance], hash[:bandwidth])
    gnu_plot(gp, ds)
  end
  Gnuplot.open do |gp|
    ds = create_data_set('d-delay', 'd[m]', 'delay[ms]', hash[:distance], hash[:delay])
    gnu_plot(gp, ds)
  end
  Gnuplot.open do |gp|
    ds = create_data_set('d-jitter', 'd[m]', 'jitter[ms]', hash[:distance], hash[:jitter])
    gnu_plot(gp, ds)
  end
  Gnuplot.open do |gp|
    ds =  create_data_set('d-fer', 'd[m]', 'FER', hash[:distance], hash[:FER])
    gnu_plot(gp, ds)
  end

end
end
