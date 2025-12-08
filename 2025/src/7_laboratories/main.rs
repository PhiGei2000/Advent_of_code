use std::{
    fmt::{Display, Write},
    fs::File,
    io::Read,
    ops::{Add, AddAssign},
};

#[derive(PartialEq, Eq, Clone, Copy)]
enum TachyonState {
    EMPTY,
    BEAM(usize),
    SPLITTER(usize),
    START(usize),
}

impl Display for TachyonState {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_char(match self {
            TachyonState::EMPTY => '.',
            TachyonState::BEAM(_) => '|',
            TachyonState::SPLITTER(_) => '^',
            TachyonState::START(_) => 'S',
        })
    }
}

impl Add for TachyonState {
    type Output = TachyonState;

    fn add(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (Self::EMPTY, other) | (other, Self::EMPTY) => other,
            (Self::BEAM(t1), Self::BEAM(t2)) => Self::BEAM(t1 + t2),
            (Self::SPLITTER(t1), Self::BEAM(t2)) => Self::SPLITTER(t1 + t2),
            _ => panic!(),
        }
    }
}

impl AddAssign for TachyonState {
    fn add_assign(&mut self, rhs: Self) {
        match (self.clone(), rhs) {
            (Self::EMPTY, other) => *self = other,
            // (other, Self::EMPTY) =>,
            (Self::BEAM(t1), Self::BEAM(t2)) => *self = Self::BEAM(t1.to_owned() + t2),
            (Self::START(_), _) | (_, Self::START(_)) => panic!(),
            // (Self::SPLITTER(t), _) =>
            (_, Self::SPLITTER(t)) => *self = Self::SPLITTER(t),
            (Self::SPLITTER(t1), Self::BEAM(t2)) => *self = Self::SPLITTER(t1 + t2),
            _ => {}
        }
    }
}

struct TachyonManifold {
    states: Vec<Vec<TachyonState>>,
}

impl From<String> for TachyonManifold {
    fn from(value: String) -> Self {
        let lines = value.trim().split("\r\n");

        TachyonManifold {
            states: lines
                .map(|line| {
                    line.trim()
                        .chars()
                        .map(|character| match character {
                            '.' => TachyonState::EMPTY,
                            '|' => TachyonState::BEAM(0),
                            '^' => TachyonState::SPLITTER(0),
                            'S' => TachyonState::START(1),
                            ch => panic!("Found character {}", ch),
                        })
                        .collect::<Vec<TachyonState>>()
                })
                .collect(),
        }
    }
}

impl Display for TachyonManifold {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in &self.states {
            for state in row {
                write!(f, "{}", state)?;
            }
            write!(f, "\r\n")?
        }

        Ok(())
    }
}

impl TachyonManifold {
    fn calculate_beam(&mut self) -> u32 {
        let mut splits_count = 0;

        if let Some(start_pos) = self.states.iter().enumerate().find_map(|(row_index, row)| {
            row.iter()
                .enumerate()
                .find_map(|(col_index, state)| match state {
                    TachyonState::START(_) => Some((row_index, col_index)),
                    _ => None,
                })
        }) {
            let mut y = start_pos.0 + 1;
            let mut x_values = vec![start_pos.1];

            while y < self.states.len() {
                let mut i = 0;
                let mut next_x_values = vec![];

                while i < x_values.len() {
                    let x = x_values[i];
                    let prev_state = match self.states[y - 1][x] {
                        TachyonState::START(i) => TachyonState::BEAM(i),
                        other => other,
                    };

                    self.states[y][x] += prev_state;

                    match self.states[y][x] {
                        TachyonState::SPLITTER(timelines_count) => {
                            splits_count += 1;

                            if x > 0 {
                                if !next_x_values.contains(&(x - 1)) {
                                    next_x_values.push(x - 1);
                                }
                                self.states[y][x - 1] += TachyonState::BEAM(timelines_count);
                            }
                            if x < self.states[0].len() - 1 {
                                if !next_x_values.contains(&(x + 1)) {
                                    next_x_values.push(x + 1);
                                }
                                self.states[y][x + 1] += TachyonState::BEAM(timelines_count);
                            }
                        }
                        _ => {
                            if !next_x_values.contains(&x) {
                                next_x_values.push(x);
                            }
                        }
                    }
                    i += 1
                }

                x_values = next_x_values;
                y += 1
            }
        }

        splits_count
    }

    fn count_timelines(&self) -> usize {
        if let Some(last) = self.states.iter().last() {
            last.iter()
                .map(|&state| match state {
                    TachyonState::BEAM(timelines_count) => timelines_count,
                    _ => 0,
                })
                .sum()
        } else {
            0
        }
    }

    fn print_timelines(&self) {
        for row in self.states.iter() {
            println!(
                "{:?}",
                row.iter()
                    .map(|&state| match state {
                        TachyonState::BEAM(i) | TachyonState::START(i) => i,
                        _ => 0,
                    })
                    .collect::<Vec<usize>>()
            )
        }
    }
}

fn main() -> std::io::Result<()> {
    // let input = "
    // .......S.......\r\n
    // ...............\r\n
    // .......^.......\r\n
    // ...............\r\n
    // ......^.^......\r\n
    // ...............\r\n
    // .....^.^.^.....\r\n
    // ...............\r\n
    // ....^.^...^....\r\n
    // ...............\r\n
    // ...^.^...^.^...\r\n
    // ...............\r\n
    // ..^...^.....^..\r\n
    // ...............\r\n
    // .^.^.^.^.^...^.\r\n
    // ..............."
    //     .to_string();
    let mut file = File::open("src/7_laboratories/data.txt")?;
    let mut input = String::new();
    let _ = file.read_to_string(&mut input);

    let mut tachyon_manifold = TachyonManifold::from(input);

    let total_splits = tachyon_manifold.calculate_beam();

    println!("{}", tachyon_manifold);
    println!("{total_splits} splits count");

    // tachyonManifold.print_timelines();
    let timelines_count = tachyon_manifold.count_timelines();

    println!("{timelines_count} timelines count");
    Ok(())
}
