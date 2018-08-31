import lsh
import time

import random
from operator import itemgetter


class LSHTester:
    """
    grid search over LSH parameters, evaluating by finding the specified
    number of nearest neighbours for the supplied queries from the supplied points
    """

    def __init__(self, points, queries, num_neighbours):
        self.points = points
        self.queries = queries
        self.num_neighbours = num_neighbours
        #  print(points)
        #  print(queries)
        #  print(num_neighbours)

    def run(self, name, metric, hash_family, k_vals, L_vals, radius):
        """
        name: name of test
        metric: distance metric for nearest neighbour computation
        hash_family: hash family for LSH
        k_vals: numbers of hashes to concatenate in each hash function to try in grid search
        L_vals: numbers of hash functions/tables to try in grid search
        """

        print(name)
        #  print('L\tk\tacc\ttouch')
        print('L\tk\tloop_ave\trate_avg\telapsed_avg')
        for k in k_vals:  # concatenating more hash functions increases selectivity
            l2lsh = lsh.LSHIndex(hash_family, k, 0, radius)

            for L in L_vals:  # using more hash tables increases recall
                l2lsh.resize(L)
                l2lsh.index(self.points)

                start = time.perf_counter()
                sum_loop = 0
                sum_hit_rate = 0
                #  print("querys start")
                for p in points:
                    #  print("query {0}".format(p))

                    candidates = l2lsh.query(
                        p,
                        metric,
                    )

                    exact_points = []
                    for p2 in points[:num_points]:
                        if p != p2:
                            if lsh.L2_norm(p, p2) <= radius:
                                exact_points.append(p2)
                    #  print("anser: {0}".format(exact_points))

                    ori_candi_len = len(candidates)

                    temp_candi = []
                    for i in candidates:
                        if metric(p, points[i]) <= radius and metric(
                                p, points[i]) != 0.0:
                            temp_candi.append(i)

                    candidates = temp_candi

                    candidates = [(ix, self.points[ix],
                                   metric(p, self.points[ix]))
                                  for ix in candidates]

                    #  print("result: {0}".format(candidates))

                    if len(exact_points) != 0:
                        hit_rate = self.hit(candidates,
                                            exact_points) / len(exact_points)
                    else:
                        hit_rate = 1.0

                    sum_hit_rate += hit_rate

                    #  candi_len = len(candidates)
                    loop_num_in_query = ori_candi_len + L

                    sum_loop += loop_num_in_query

                    #  print("---------------------------")
                    #  print(
                    #      "exact {0} \thit {1}\thit_rate {4:.2f}\nori_candi_num {2}\tcandi_len {5}\tloop_in_query {3}".
                    #      format(
                    #          len(exact_points),
                    #          self.hit(candidates, exact_points),
                    #          ori_candi_len,
                    #          loop_num_in_query,
                    #          hit_rate,
                    #          candi_len,
                    #      ))

                    #  print()

                elapsed = time.perf_counter() - start
                #  print("querys finish")
                #  print("elapsed {0} \tavg {1}".format(
                #      time.time() - start,
                #      (time.time() - start) / p_index,
                #  ))
                #  print("hit_rate_avg {0}".format(sum_hit_rate / p_index,))
                #  print()
                #  print(l2lsh.query([100, 100], metric, self.num_neighbours))

                #  correct = 0
                #  for q, hits in zip(self.queries, exact_hits):
                #      lsh_hits = [
                #          ix for ix, dist in l2lsh.query(q, metric,
                #                             l2          self.num_neighbours + 1)
                #      ]
                #      if lsh_hits == hits:
                #          correct += 1

                points_len = len(points)
                print("{0}\t{1}\t{2:.2f}\t\t{3:.5f}\t\t{4:.9f}".format(
                    L,
                    k,
                    sum_loop / points_len,
                    sum_hit_rate / points_len,
                    elapsed / points_len,
                ))
                #  float(l2lsh.get_avg_touched()) / len(self.points)))
            print()

    def linear(self, q, metric, max_results):
        """ brute force search by linear scan """
        candidates = [(ix, metric(q, p)) for ix, p in enumerate(self.points)]
        return sorted(candidates, key=itemgetter(1))[:max_results]

    def hit(self, cand, exact):
        hit_num = 0
        for d in exact:
            for c in cand:
                if d == c[1]:
                    hit_num += 1
                    break
        return hit_num


if __name__ == "__main__":

    # create a test dataset of vectors of non-negative integers
    d = 2
    xmax = 10000
    num_points = 512
    points = [
        [random.randint(0, xmax) for i in range(d)] for j in range(num_points)
    ]

    # seed the dataset with a fixed number of nearest neighbours
    # within a given small "radius"
    num_neighbours = 1000
    radius = 243
    #  for point in points[:num_points]:
    #      for i in range(num_neighbours):
    #          points.append([x + random.uniform(-radius, radius) for x in point])

    #  print(points)
    #  print(ans_points)
    # test lsh versus brute force comparison by running a grid
    # search for the best lsh parameter values for each family
    tester = LSHTester(points, points[:int(num_points)], num_neighbours)

    args = {
        'name': 'L2',
        'metric': lsh.L2_norm,
        'hash_family': lsh.L2HashFamily(radius * 1.1, d),
        'k_vals': [1, 2, 3, 4, 5, 6, 7, 8],
        'L_vals': [2, 4, 8, 16, 32, 64, 128],
        #  'k_vals': [4],
        #  'L_vals': [32],
        'radius': radius,
    }
    tester.run(**args)

    #  args = {
    #      'name': 'L1',
    #      'metric': L1_norm,
    #      'hash_family': L1HashFamily(10 * radius, d),
    #      'k_vals': [2, 4, 8],
    #      'L_vals': [2, 4, 8, 16]
    #  }
    #  tester.run(**args)

    #  args = {
    #      'name': 'cosine',
    #      'metric': cosine_distance,
    #      'hash_family': CosineHashFamily(d),
    #      'k_vals': [16, 32, 64],
    #      'L_vals': [2, 4, 8, 16]
    #  }
    #  tester.run(**args)
