use std::fmt::{Debug, Display};

struct Machine {
    indicator_lights: Vec<f32>,
    buttons: Vec<Vec<f32>>,
    joltages: Vec<u32>,
}

#[derive(Clone)]
struct Matrix {
    entries: Vec<Vec<f32>>,
}

impl Display for Matrix {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in self.entries.iter() {
            writeln!(f, "{row:?}")?;
        }

        Ok(())
    }
}

impl Debug for Matrix {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in self.entries.iter() {
            writeln!(f, "{row:?}")?;
        }

        Ok(())
    }
}

impl Matrix {
    fn new(height: usize, width: usize) -> Self {
        Self {
            entries: (0..height)
                .map(|i| {
                    let mut row = Vec::<f32>::new();
                    row.resize(width.into(), 0.0);
                    row[i] = 1.0f32;
                    row
                })
                .collect(),
        }
    }

    fn cols_count(&self) -> usize {
        self.entries
            .first()
            .and_then(|row| Some(row.len()))
            .unwrap_or_default()
    }

    fn rows_count(&self) -> usize {
        self.entries.len()
    }

    fn zero(height: usize, width: usize) -> Self {
        Self {
            entries: (0..height)
                .map(|i| {
                    let mut row = Vec::<f32>::new();
                    row.resize(width.into(), 0.0);
                    row
                })
                .collect(),
        }
    }

    fn from_cols(cols: Vec<Vec<f32>>) -> Self {
        let mat = Self { entries: cols };
        mat.transpose()
    }

    fn swap_cols(&mut self, i: usize, j: usize) {
        for row in self.entries.iter_mut() {
            row.swap(i, j);
        }
    }

    fn transpose(&self) -> Self {
        let cols_count = self.cols_count();
        let rows_count = self.rows_count();

        let mut result = Self::zero(cols_count, rows_count);
        for (i, row) in self.entries.iter().enumerate() {
            for (j, &val) in row.iter().enumerate() {
                result.entries[j][i] = val;
            }
        }

        result
    }

    fn mul_row(&mut self, i: usize, factor: f32) {
        for j in 0..self.cols_count() {
            self.entries[i][j] *= factor;
        }
    }

    fn mul_vec(&self, v: Vec<f32>) -> Vec<f32> {
        assert_eq!(self.rows_count(), v.len());

        self.entries
            .iter()
            .map(|row| row.iter().zip(v.iter()).map(|(&a, b)| a * b).sum())
            .collect()
    }

    fn mul(&self, mat: Self) -> Self {
        assert_eq!(self.cols_count(), mat.rows_count());

        let mut result = Matrix::zero(self.rows_count(), mat.cols_count());
        for i in 0..self.rows_count() {
            for j in 0..mat.cols_count() {
                for k in 0..self.cols_count() {
                    result.entries[i][j] += self.entries[i][k] * mat.entries[k][j];
                }
            }
        }

        result
    }

    fn split_by_col(&self, col: usize) -> (Self, Self) {
        assert_eq!(col <= self.cols_count(), true);

        let mut result = (Matrix { entries: vec![] }, Matrix { entries: vec![] });
        for row in self.entries.iter() {
            result
                .0
                .entries
                .push(row.iter().take(col).map(|&val| val).collect());
            result
                .1
                .entries
                .push(row.iter().skip(col).map(|&val| val).collect());
        }

        result
    }

    fn gauss(&self, v: &Vec<f32>) -> (Self, Vec<f32>) {
        // assert!(self.rows_count() <= self.cols_count());
        // assert!(v.len() <= self.cols_count());

        let mut result = self.clone();

        let n = self.rows_count();
        if self.rows_count() < self.cols_count() {
            result.entries.resize(v.len(), vec![0.0; self.cols_count()]);
        } else if self.rows_count() > self.cols_count() {
            result
                .entries
                .iter_mut()
                .for_each(|entry| entry.resize(self.rows_count(), 0.0));
        }

        let mut v = v.clone();
        if v.len() < self.cols_count() {
            v.resize(self.cols_count(), 0.0);
        }

        let mut i = 0;
        while i < n {
            // iterate through columns
            if let Some((p_i, pivot)) = result
                .entries
                .iter()
                .enumerate()
                .skip(i)
                .find_map(|(p_i, r)| (r[i] != 0.0).then(|| (p_i, r[i])))
            {
                if i != p_i {
                    result.entries.swap(i, p_i);
                }

                for j in i + 1..self.rows_count() {
                    // following rows
                    if result.entries[j][i] != 0.0 {
                        for k in 0..self.cols_count() {
                            // following columns
                            result.entries[j][k] -= result.entries[i][k];
                        }
                        v[j] -= v[i];
                    }
                }

                for k in 0..self.cols_count() {
                    result.entries[i][k] *= 1.0 / pivot;
                }

                i += 1;
            } else {
                // only zeros in column
                if let Some(next_i) = result.entries[i]
                    .iter()
                    .enumerate()
                    .skip(i + 1)
                    .find_map(|(index, &val)| (val != 0.0).then(|| index))
                {
                    result.swap_cols(i, next_i);
                    if i < v.len() && next_i < v.len() {
                        v.swap(i, next_i);
                    }
                } else {
                    break;
                }
            }
        }

        // clear columns
        for i in (0..n).rev() {
            // diagonal
            if result.entries[i][i] != 0.0 {
                for j in (0..i).rev() {
                    // entries above diagonal
                    let factor = result.entries[j][i] / result.entries[i][i];
                    for k in 0..self.cols_count() {
                        result.entries[j][k] -= factor * result.entries[i][k];
                    }
                    v[j] -= factor * v[i];
                }
            }
        }

        (result, v)
    }
}

impl From<String> for Machine {
    fn from(value: String) -> Self {
        let parts = value.trim().split_whitespace();

        let mut indicator_lights: Vec<f32> = vec![];
        let mut buttons: Vec<Vec<f32>> = vec![];
        let mut joltages = vec![];
        let mut buttons_count = 0;

        for part in parts {
            match part.chars().nth(0) {
                Some('[') => {
                    indicator_lights = part[1..part.len()]
                        .chars()
                        .filter_map(|c| match c {
                            '.' => Some(0.0),
                            '#' => Some(1.0),
                            _ => None,
                        })
                        .collect();

                    buttons_count = indicator_lights.len();
                }
                Some('(') => {
                    let mut result = vec![0.0; buttons_count];
                    part[1..part.len() - 1]
                        .split(',')
                        .filter_map(|num| num.parse::<usize>().ok())
                        .for_each(|index| result[index] = 1.0);

                    buttons.push(result);
                }
                Some('{') => {
                    joltages = part[1..part.len() - 1]
                        .split(',')
                        .filter_map(|num| num.parse::<u32>().ok())
                        .collect()
                }
                _ => {}
            }
        }

        Machine {
            indicator_lights,
            buttons,
            joltages,
        }
    }
}

impl Machine {
    // fn get_fewest_presses(&self) -> u64 {
    //     let mut states = self.buttons.clone();
    //     let mut counter = 1;

    //     while !states.contains(&self.indicator_lights) {
    //         let next_states = states
    //             .iter()
    //             .map(|state| {
    //                 self.buttons
    //                     .iter()
    //                     .filter_map(|button| match state ^ button {
    //                         0 => None,
    //                         new_state => Some(new_state),
    //                     })
    //                     .collect::<Vec<u32>>()
    //             })
    //             .flatten()
    //             .collect::<Vec<u32>>();

    //         states = next_states;
    //         counter += 1;
    //     }

    //     counter
    // }

    fn get_fewest_presses(&self) -> u64 {
        let mat = Matrix::from_cols(self.buttons.clone());

        let (mut inv, mut b) = mat.gauss(&self.indicator_lights);
        inv.entries.iter_mut().for_each(|row| {
            row.iter_mut()
                .for_each(|entry| *entry = entry.rem_euclid(2.0).abs())
        });

        b.iter_mut().for_each(|v| *v = v.rem_euclid(2.0));

        println!("{inv:?}");
        println!("{b:?}");

        0
    }
}

fn main() {
    let input = "[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}\r\n
        [...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}\r\n
        [.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}\r\n
        "
    .to_string();
    // let input = std::fs::read_to_string("src/10_factory/data.txt").unwrap();

    let machines = input
        .trim()
        .split("\r\n")
        .map(|line| Machine::from(line.to_string()))
        .collect::<Vec<Machine>>();

    machines.iter().for_each(|machine| {
        machine.get_fewest_presses();
    });

    // let fewest_total_presses = machines
    //     .iter()
    //     .map(|machine| {
    //         let fewest_presses = machine.get_fewest_presses();

    //         fewest_presses as u64
    //     })
    //     .sum::<u64>();

    // println!("Fewest total presses: {fewest_total_presses}")
}
