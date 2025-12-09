use std::{
    fs::File,
    io::Read,
    ops::{Add, Sub},
};

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
struct Vec2 {
    x: i64,
    y: i64,
}

impl Add for Vec2 {
    type Output = Self;

    fn add(self, rhs: Self) -> Self::Output {
        Vec2 {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
        }
    }
}

impl Sub for Vec2 {
    type Output = Self;

    fn sub(self, rhs: Self) -> Self::Output {
        Vec2 {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
        }
    }
}

impl Vec2 {
    fn dot(&self, rhs: &Self) -> i64 {
        self.x * rhs.x + self.y * rhs.y
    }

    fn cross(&self, rhs: &Self) -> i64 {
        self.x * rhs.y - self.y * rhs.x
    }
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
struct Rectangle {
    x1: i64,
    x2: i64,
    y1: i64,
    y2: i64,
}

impl Rectangle {
    fn from_points(p1: Vec2, p2: Vec2) -> Rectangle {
        Rectangle {
            x1: p1.x.min(p2.x),
            x2: p1.x.max(p2.x),
            y1: p1.y.min(p2.y),
            y2: p1.y.max(p2.y),
        }
    }

    fn get_area(&self) -> u64 {
        (self.x1.abs_diff(self.x2) + 1) * (self.y1.abs_diff(self.y2) + 1)
    }

    fn intersects(&self, other: &Rectangle) -> bool {
        self.x1 < other.x2 && other.x1 < self.x2 && self.y1 < other.y2 && other.y1 < self.y2
    }
}

#[derive(Clone, Copy, Debug)]
struct Line {
    start: Vec2,
    end: Vec2,
}

#[derive(PartialEq, Eq, Debug)]
enum Orientation {
    LEFT,
    RIGHT,
    UNDEFINED,
}

impl Line {
    fn get_distance(&self, point: &Vec2) -> Option<(Orientation, f64)> {
        let d1 = self.end - self.start;
        let d2 = *point - self.start;

        let cross = d1.cross(&d2);
        let dot = d1.dot(&d2);
        let l_square = d1.dot(&d1);
        let perp_dist = cross as f64 / (l_square as f64).sqrt();

        if 0 <= dot && dot <= l_square {
            if cross == 0 {
                Some((Orientation::UNDEFINED, perp_dist))
            } else if cross < 0 {
                Some((Orientation::LEFT, perp_dist.abs()))
            } else {
                Some((Orientation::RIGHT, perp_dist.abs()))
            }
        } else {
            None
        }
    }
}

struct TileFloor {
    red_tiles: Vec<Vec2>,
    orientation: Orientation,
}

impl From<String> for TileFloor {
    fn from(value: String) -> Self {
        let red_tiles = value
            .trim()
            .split("\r\n")
            .filter_map(|line| {
                line.trim().split_once(",").and_then(|(x_str, y_str)| {
                    Some(Vec2 {
                        x: x_str.parse::<i64>().unwrap(),
                        y: y_str.parse::<i64>().unwrap(),
                    })
                })
            })
            .collect::<Vec<Vec2>>();

        let (orientation, _) = Line {
            start: red_tiles[0],
            end: red_tiles[1],
        }
        .get_distance(&red_tiles[2])
        .unwrap();

        TileFloor {
            red_tiles,
            orientation,
        }
    }
}

impl TileFloor {
    fn get_rectangles(&self) -> Vec<Rectangle> {
        let mut pairs = Vec::<Rectangle>::new();
        pairs.reserve(self.red_tiles.len() * (self.red_tiles.len() - 1) / 2);

        for i in 0..self.red_tiles.len() {
            for j in i + 1..self.red_tiles.len() {
                pairs.push(Rectangle {
                    x1: self.red_tiles[i].x.min(self.red_tiles[j].x),
                    y1: self.red_tiles[i].y.min(self.red_tiles[j].y),
                    x2: self.red_tiles[i].x.max(self.red_tiles[j].x),
                    y2: self.red_tiles[i].y.max(self.red_tiles[j].y),
                });
            }
        }

        pairs
    }

    fn find_largest_rectangle(&self) -> Option<u64> {
        self.get_rectangles()
            .iter()
            .map(|rect| rect.get_area())
            .max()
    }

    fn get_green_rectangles(&self) -> Vec<Rectangle> {
        let contour = self.get_contour();
        let rectangles = self.get_rectangles();

        let empty_area = contour
            .iter()
            .enumerate()
            .filter_map(|(i, line)| {
                let p = contour[(i + 1) % contour.len()].end;

                match line.get_distance(&p).unwrap().0 {
                    orientation if orientation != self.orientation => {
                        Some(Rectangle::from_points(line.start, p))
                    }
                    _ => None,
                }
            })
            .collect::<Vec<Rectangle>>();

        rectangles
            .iter()
            .filter_map(|rectangle| {
                self.inside_green_area(rectangle, &empty_area)
                    .then(|| *rectangle)
            })
            .collect()
    }

    fn find_largest_green_rectangle(&self) -> Option<u64> {
        let green_rectangles = self.get_green_rectangles();

        green_rectangles.iter().map(|rect| rect.get_area()).max()
    }

    fn get_contour(&self) -> Vec<Line> {
        self.red_tiles
            .iter()
            .enumerate()
            .map(|(index, &point)| Line {
                start: point,
                end: self.red_tiles[(index + 1) % self.red_tiles.len()],
            })
            .collect::<Vec<Line>>()
    }

    fn inside_green_area(&self, rect: &Rectangle, empty_area: &Vec<Rectangle>) -> bool {
        empty_area.iter().all(|area| !rect.intersects(area))
    }
}

fn main() -> std::io::Result<()> {
    // let input = "
    //     7,1\r\n
    //     11,1\r\n
    //     11,7\r\n
    //     9,7\r\n
    //     9,5\r\n
    //     2,5\r\n
    //     2,3\r\n
    //     7,3\r\n
    // "
    // .to_string();
    let mut file = File::open("src/9_movie_theater/data.txt")?;
    let mut input = String::new();
    let _ = file.read_to_string(&mut input);

    let tile_floor = TileFloor::from(input);

    if let Some(max_area) = tile_floor.find_largest_rectangle() {
        println!("Max area: {max_area}")
    }

    if let Some(max_green_area) = tile_floor.find_largest_green_rectangle() {
        println!("Max area of rectangle inside green area: {max_green_area}")
    }

    Ok(())
}
