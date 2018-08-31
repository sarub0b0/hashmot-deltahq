"""
LSH Locality Sensitive Hashing
- indexing for nearest neighbour searches in sublinear time

simple tutorial implementation based on
A. Andoni and P. Indyk, "Near-optimal hashing algorithms for approximate nearest neighbor in high dimensions"
http://people.csail.mit.edu/indyk/p117-andoni.pdf
"""

import pprint
import random
from collections import defaultdict

#  from operator import itemgetter


class LSHIndex:
    def __init__(self, hash_family, k, L, radius):
        self.hash_family = hash_family
        self.k = k
        self.L = 0
        self.hash_tables = []
        self.resize(L)
        self.radius = radius

    def resize(self, L):
        """ update the number of hash tables to be used """
        if L < self.L:
            self.hash_tables = self.hash_tables[:L]
        else:
            # initialise a new hash table for each hash function
            hash_funcs = [[
                self.hash_family.create_hash_func() for h in range(self.k)
            ] for l in range(self.L, L)]

            self.hash_tables.extend(
                [(g, defaultdict(lambda: [])) for g in hash_funcs])

    def hash(self, g, p):
        pprint.pprint(self.hash_family.combine([h.hash(p) for h in g]))
        return self.hash_family.combine([h.hash(p) for h in g])

    def index(self, points):
        """ index the supplied points """
        self.points = points
        for g, table in self.hash_tables:
            for ix, p in enumerate(self.points):
                table[self.hash(g, p)].append(ix)
        # reset stats
        self.tot_touched = 0
        self.num_queries = 0

    def update(self, id_list, points):
        for g, table in self.hash_tables:
            for i in id_list:
                tbl = table[self.hash(g, self.points[i])]
                if tbl and i in tbl:
                    tbl.remove(i)

        self.points = points

        for g, table in self.hash_tables:
            for i in id_list:
                table[self.hash(g, self.points[i])].append(i)

    def query(self, q, metric):
        """ find the max_results closest indexed points to q according to the supplied metric """
        candidates = set()

        for g, table in self.hash_tables:
            matches = table.get(self.hash(g, q), [])
            candidates.update(matches)

        # update stats
        self.tot_touched += len(candidates)
        self.num_queries += 1
        # rerank candidates

        temp_candi = []
        for i in candidates:
            if metric(q, self.points[i]) <= self.radius and metric(
                    q, self.points[i]) != 0.0:
                temp_candi.append(i)

        candidates = temp_candi
        return candidates

    def get_avg_touched(self):
        """ mean number of candidates inspected per query """
        return self.tot_touched / self.num_queries


class L1HashFamily:
    def __init__(self, w, d):
        self.w = w
        self.d = d

    def create_hash_func(self):
        # each L1Hash is initialised with a different random partition vector
        return L1Hash(self.rand_partition(), self.w)

    def rand_partition(self):
        return [random.uniform(0, self.w) for i in range(self.d)]

    def combine(self, hashes):
        """
        combine hash values naively with str()
        - in a real implementation we can mix the values so they map to integer keys
        into a conventional map table
        """
        return str(hashes)


class L1Hash:
    def __init__(self, S, w):
        self.S = S
        self.w = w

    def hash(self, vec):
        # use str() as a naive way of forming a single value
        return str([int((vec[i] - s) / self.w) for i, s in enumerate(self.S)])


def L1_norm(u, v):
    return sum(abs(u[i] - v[i]) for i in range(len(u)))


def dot(u, v):
    return sum(ux * vx for ux, vx in zip(u, v))


class L2HashFamily:
    def __init__(self, w, d):
        self.w = w
        self.d = d

    def create_hash_func(self):
        # each L2Hash is initialised with a different random projection vector and offset
        return L2Hash(self.rand_vec(), self.rand_offset(), self.w)

    def rand_vec(self):
        return [random.gauss(0, 1) for i in range(self.d)]

    def rand_offset(self):
        return random.uniform(0, self.w)

    def combine(self, hashes):
        """
        combine hash values naively with str()
        - in a real implementation we can mix the values so they map to integer keys
        into a conventional map table
        """
        return str(hashes)


class L2Hash:
    def __init__(self, r, b, w):
        self.r = r
        self.b = b
        self.w = w

    def hash(self, vec):
        print(int((dot(vec, self.r) + self.b) / self.w), vec, self.r)
        return int((dot(vec, self.r) + self.b) / self.w)


def L2_norm(u, v):
    #  return sum((ux - vx)**2 for ux, vx in zip(u, v))**0.5
    return sum((ux - vx)**2 for ux, vx in zip(u, v))**0.5


class CosineHashFamily:
    def __init__(self, d):
        self.d = d

    def create_hash_func(self):
        # each CosineHash is initialised with a random projection vector
        return CosineHash(self.rand_vec())

    def rand_vec(self):
        return [random.gauss(0, 1) for i in range(self.d)]

    def combine(self, hashes):
        """ combine by treating as a bitvector """
        return sum(2**i if h > 0 else 0 for i, h in enumerate(hashes))


class CosineHash:
    def __init__(self, r):
        self.r = r

    def hash(self, vec):
        return self.sgn(dot(vec, self.r))

    def sgn(self, x):
        return int(x > 0)


def cosine_distance(u, v):
    return 1 - dot(u, v) / (dot(u, u) * dot(v, v))**0.5
