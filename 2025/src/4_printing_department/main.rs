use std::{fs::File, io::Read};

struct PaperStorage {
    grid: Vec<Vec<bool>>,
}

impl From<String> for PaperStorage {
    fn from(value: String) -> Self {
        let rows = value.split_whitespace();
        let mut grid: Vec<Vec<bool>> = vec![];

        for row in rows {
            let parsed_row: Vec<bool> = row
                .chars()
                .map(|char| match char {
                    '@' => true,
                    '.' => false,
                    _ => panic!(),
                })
                .collect();

            grid.push(parsed_row);
        }

        PaperStorage { grid }
    }
}

impl PaperStorage {
    fn occupied(&self, x: usize, y: usize) -> bool {
        self.grid[x][y]
    }

    fn accessable(&self, x: usize, y: usize) -> bool {
        let mut neighbour_rolls = 0;

        let (width, height) = self.get_size();
        let x_min = match x {
            0 => 0,
            _ => x - 1,
        };
        let x_max = match x {
            x if x > width - 1 => panic!(),
            x if x == width - 1 => width,
            _ => x + 2,
        };
        let y_min = match y {
            0 => 0,
            _ => y - 1,
        };
        let y_max = match y {
            y if y > height - 1 => panic!(),
            y if y == height - 1 => height,
            _ => y + 2,
        };

        for i in x_min..x_max {
            for j in y_min..y_max {
                if i == x && j == y {
                    continue;
                }

                if self.grid[i][j] {
                    neighbour_rolls += 1;
                }
            }
        }

        neighbour_rolls < 4
    }

    fn remove(&mut self, x: usize, y: usize) {
        self.grid[x][y] = false;
    }

    fn get_size(&self) -> (usize, usize) {
        (self.grid.len(), self.grid[0].len())
    }
}

fn main() -> std::io::Result<()> {
    // let input = "..@@.@@@@.\r\n@@@.@.@.@@\r\n@@@@@.@.@@\r\n@.@@@@..@.\r\n@@.@@@@.@@\r\n.@@@@@@@.@\r\n.@.@.@.@@@\r\n@.@@@.@@@@\r\n.@@@@@@@@.\r\n@.@.@@@.@.".to_string();
    let mut file = File::open("src/4_printing_department/data.txt")?;
    let mut input = String::new();
    let _ = file.read_to_string(&mut input);

    let mut storage = PaperStorage::from(input);
    let (width, height) = storage.get_size();

    let mut removed_rolls = 0;

    loop {
        let mut removeable_rolls: Vec<(usize, usize)> = vec![];

        for x in 0..width {
            for y in 0..height {
                if storage.occupied(x, y) {
                    if storage.accessable(x, y) {
                        removeable_rolls.push((x, y));
                    }
                }
            }
        }
        let accessable_rolls = removeable_rolls.len();

        for (x, y) in removeable_rolls {
            storage.remove(x, y);
        }
        removed_rolls += accessable_rolls;

        if accessable_rolls == 0 {
            break;
        }
    }

    println!("{removed_rolls} rolls removed!");
    Ok(())
}
