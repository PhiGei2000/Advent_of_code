use std::fmt::{Debug, Display};

struct Machine {
    indicator_lights: u32,
    buttons: Vec<u32>,
    joltages: Vec<u32>,
    buttons_count: usize,
}

#[derive(Clone)]
struct F2Matrix {
    entries: Vec<u32>,
    height: usize,
    width: usize,
}

impl Display for F2Matrix {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in self.entries.iter() {
            write!(f, "[")?;
            for i in 0..self.width {
                write!(f, " {} ", (row >> i) & 1)?;
            }
            writeln!(f, "]")?;
        }

        Ok(())
    }
}

impl Debug for F2Matrix {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in self.entries.iter() {
            write!(f, "[")?;
            for i in 0..self.width {
                write!(f, " {} ", (row >> i) & 1)?;
            }
            writeln!(f, "]")?;
        }

        Ok(())
    }
}

impl F2Matrix {
    fn from_cols(cols: Vec<u32>, height: usize) -> Self {
        let width = cols.len();

        let mut entries = vec![0; height];
        for i in 0..height {
            for j in 0..width {
                entries[i] |= ((cols[j] >> i) & 1) << j;
            }
        }

        F2Matrix {
            entries,
            height,
            width,
        }
    }

    fn swap_cols(&mut self, i: usize, j: usize) {
        for row in self.entries.iter_mut() {
            let a = (*row >> i) & 1; // a = row[i]
            *row &= !(1 << i); // row[i] = 0 
            *row |= ((*row >> j) & 1) << i; // row[i] == row[j]
            *row &= !(1 << j); // row[j] = 0
            *row |= a << j; // row[j] = a
        }
    }

    fn gauss(&self, v: u32) -> (Self, u32) {
        let mut result = self.clone();

        if self.width < self.height {
            result.width = self.height;
        } else if self.width > self.height {
            result.entries.resize(self.width, 0);
            result.height = self.width;
        }
        let n = result.height;
        let mut rows = (0..result.height).collect::<Vec<usize>>();
        let mut linear_dependent_cols: Vec<usize> = vec![];

        let mut v = v;

        let mut r = 0;
        let mut c = 0;
        while r < n && c < n {
            // iterate through columns
            if let Some(p_i) = result
                .entries
                .iter()
                .enumerate()
                .skip(r)
                .find_map(|(p_i, row)| ((row >> c) & 1 != 0).then(|| p_i))
            {
                if r != p_i {
                    result.entries.swap(r, p_i);

                    let a = (v >> r) & 1;
                    v &= !(1 << r);
                    v |= ((v >> p_i) & 1) << r;
                    v &= !(1 << p_i);
                    v |= a << p_i;
                }

                for j in (r + 1)..result.height {
                    // following rows
                    if (result.entries[j] >> c) & 1 != 0 {
                        // following columns
                        result.entries[j] ^= result.entries[r];

                        v ^= ((v >> c) & 1) << j;
                    }
                }

                c += 1;
                r += 1;
            } else {
                c += 1;
                linear_dependent_cols.push(c);
            }
        }

        // clear columns
        for i in (0..n).rev() {
            // diagonal
            if (result.entries[i] >> i) & 1 != 0 {
                for j in (0..i).rev() {
                    // entries above diagonal
                    if (result.entries[j] >> i) & 1 != 0 {
                        result.entries[j] ^= result.entries[i];

                        v ^= ((v >> i) & 1) << j;
                    }
                }
            }
        }

        (result, v)
    }

    fn get_columns(&self, range: std::ops::Range<usize>) -> Vec<u32> {
        let mut result: Vec<u32> = vec![0; range.len()];
        self.entries.iter().enumerate().for_each(|(j, row)| {
            for i in range.clone() {
                result[i - range.start] |= ((row >> i) & 1) << j;
            }
        });

        result
    }
}

impl From<String> for Machine {
    fn from(value: String) -> Self {
        let parts = value.trim().split_whitespace();

        let mut indicator_lights: u32 = 0;
        let mut buttons: Vec<u32> = vec![];
        let mut joltages = vec![];
        let mut buttons_count: usize = 0;

        for part in parts {
            match part.chars().nth(0) {
                Some('[') => {
                    indicator_lights = part[1..part.len()]
                        .chars()
                        .enumerate()
                        .filter_map(|(i, c)| match c {
                            '#' => Some(1 << i),
                            _ => None,
                        })
                        .sum();

                    buttons_count = part.len() - 2;
                }
                Some('(') => {
                    let mut result = 0;
                    part[1..part.len() - 1]
                        .split(',')
                        .filter_map(|num| num.parse::<u32>().ok())
                        .for_each(|index| result |= 1 << index);

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
            buttons_count,
        }
    }
}

impl Machine {
    fn get_min_presses(b: u32, vs: &Vec<u32>, n: usize, coefficients: &mut Vec<usize>) -> u32 {
        let mut min_presses = b.count_ones();

        let last_coeff = match coefficients.last() {
            Some(&coeff) => coeff + 1,
            None => 0,
        };

        if coefficients.len() < vs.len() {
            for i in last_coeff..vs.len() {
                coefficients.push(i);
                let presses =
                    Self::get_min_presses((b ^ vs[i]) | (1 << (n + i)), vs, n, coefficients);
                coefficients.pop();
                min_presses = min_presses.min(presses);
            }
        }
        min_presses
    }

    fn get_fewest_presses(&self) -> u32 {
        let mat = F2Matrix::from_cols(self.buttons.clone(), self.buttons_count);

        let (inv, b) = mat.gauss(self.indicator_lights);
        println!("{inv:?}");
        println!("{b:#b}");

        // solution
        let mut i = 0;
        while i < inv.entries.len() && (inv.entries[i] >> i) & 1 == 1 {
            i += 1;
        }

        let vs = inv.get_columns(i..inv.height);

        let mut coefficients: Vec<usize> = vec![];
        Self::get_min_presses(b, &vs, i, &mut coefficients)
    }

    fn get_fewest_presses_old(&self) -> u32 {
        let mut states = self.buttons.clone();
        let mut counter = 1;

        while !states.contains(&self.indicator_lights) {
            let next_states = states
                .iter()
                .map(|state| {
                    states
                        .iter()
                        .filter_map(|button| match state ^ button {
                            0 => None,
                            new_state => Some(new_state),
                        })
                        .collect::<Vec<u32>>()
                })
                .flatten()
                .collect::<Vec<u32>>();

            states = next_states;
            counter += 1;
        }

        counter
    }
}

fn main() {
    let input = "[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}\r\n
        [...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}\r\n
        [.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}\r\n
        "
    .to_string();
    // let input = std::fs::read_to_string("src/10_factory/data.txt").unwrap();
    let lines: Vec<&str> = input.trim().split("\r\n").collect();

    let machines = lines
        .iter()
        .map(|line| Machine::from(line.to_string()))
        .collect::<Vec<Machine>>();

    println!(
        "Min total presses: {}",
        machines
            .iter()
            .enumerate()
            .map(|(i, machine)| {
                let a = machine.get_fewest_presses();
                // let b = machine.get_fewest_presses_old();

                // if a != b {
                //     println!("a={a} b={b}");
                //     println!("{}", lines[i]);

                //     let mat = F2Matrix::from_cols(machine.buttons.clone(), machine.buttons_count);
                //     println!("{mat:?}");

                //     let (inv, b) = mat.gauss(machine.indicator_lights);
                //     println!("{inv:?}\r\n{b:?}");

                //     let mut i = 0;
                //     while i < inv.entries.len() && (inv.entries[i] >> i) & 1 == 1 {
                //         i += 1;
                //     }

                //     let vs = inv.get_columns(i..inv.height);

                //     let mut coefficients: Vec<usize> = vec![];
                //     Machine::get_min_presses(b, &vs, i, &mut coefficients);
                // }

                a
            })
            .sum::<u32>()
    );

    // let fewest_total_presses = machines
    //     .iter()
    //     .map(|machine| {
    //         let fewest_presses = machine.get_fewest_presses();

    //         fewest_presses as u64
    //     })
    //     .sum::<u64>();

    // println!("Fewest total presses: {fewest_total_presses}")
}
