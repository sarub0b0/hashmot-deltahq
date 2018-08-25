require 'gviz'

Graph do
  global layout: 'dot', rankdir: 'TB', margin: 0.2
  nodes fontsize: 24

  node :object, label: 'object'
  node :environment, label: 'environment'
  node :mobility, label: 'mobility'
  node :pr, label: 'received power'
  node :interference, label: 'interference'
  node :fer, label: 'frame error rate'
  node :loss, label: 'packet loss rate'
  node :retr, label: 'retransmissions'
  node :op, label: 'oparating rate'
  node :delay, label: 'delay'
  node :jitter, label: 'jitter'
  node :bandwidth, label: 'bandwidth'

  edge :object_mobility

  edge :mobility_environment

  edge :environment_pr

  edge :pr_interference, style: 'dotted'
  edge :pr_fer

  edge :interference_fer, style: 'dotted'

  edge :fer_loss
  edge :fer_retr
  edge :fer_op

  edge :op_delay
  edge :op_jitter

  edge :delay_bandwidth
  edge :jitter_bandwidth

  rank :same, :loss, :retr, :op
  rank :same, :delay, :jitter

  save(:parameter, :pdf)
end
