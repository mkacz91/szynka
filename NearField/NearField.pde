final float CONTROL_POINT_RADIUS = 10.0f;

int coarseCellSize = 10;
int coarseWidth, coarseHeight;
float[][] coarse;

PImage fine;

int sampleFactor = 4;
PVector coarseSamples[];

PVector[] controlPoints;

PVector gridCenter(int i, int j, float cellSize) { return new PVector(j + 0.5f, i + 0.5f).mult(cellSize); }
PVector coarseCenter(int i, int j) { return gridCenter(i, j, coarseCellSize); }

void ellipse(PVector c, float r) { ellipse(c.x, c.y, r, r); }

int clampIndex(int i, int size)
{
    if (i < 0)
        return 0;
    if (i >= size)
        return size - 1;
    return i;
}

float getFieldValue(float x, float y)
{
    x = x / coarseCellSize - 0.5;
    y = y / coarseCellSize - 0.5;
    float x0 = floor(x), y0 = floor(y);
    
    int i0 = clampIndex((int)y0, coarseHeight);
    int i1 = clampIndex((int)y0 + 1, coarseHeight);
    int j0 = clampIndex((int)x0, coarseWidth);
    int j1 = clampIndex((int)x0 + 1, coarseWidth);
    
    float v00 = coarse[i0][j0];
    float v01 = coarse[i1][j0];
    float v10 = coarse[i0][j1];
    float v11 = coarse[i1][j1];
    
    float ax = x - x0, bx = 1.0f - ax;
    float ay = y - y0, by = 1.0f - ay;
    
    return by * (bx * v00 + ax * v10) + ay * (bx * v01 + ax * v11);
}

float per(PVector u, PVector v) { return u.x * v.y - u.y * v.x; }
PVector span(PVector u, PVector v) { return PVector.sub(v, u); }

void setup()
{
    size(800, 600);
    fine = createImage(width, height, RGB);    
    
    controlPoints = new PVector[]
    {
        new PVector(0.30f, 0.25f),
        new PVector(0.75f, 0.51f),
        new PVector(0.20f, 0.75f)
    };
    for (PVector p : controlPoints)
    {
        p.x *= width;
        p.y *= height;
    } 
    
    coarseWidth = ceil(width / coarseCellSize);
    coarseHeight = ceil(height / coarseCellSize);
    coarse = new float[coarseHeight][coarseWidth];
    
    coarseSamples = new PVector[sampleFactor * sampleFactor];
    float cellSampleFactor = coarseCellSize / sampleFactor;
    for (int i = 0; i < sampleFactor; ++i)
    {
        for (int j = 0; j < sampleFactor; ++j)
            coarseSamples[i * sampleFactor + j] = new PVector(j + 0.5f, i + 0.5f).mult(cellSampleFactor);
    }
    
    updateCoarse();
    updateFine();
}

void draw()
{
    background(255);
    
    noStroke();
    rectMode(CORNER);
    for (int i = 0; i < coarseHeight; ++i)
    {
        for (int j = 0; j < coarseWidth; ++j)
        {
            fill(255 - coarse[i][j] * 255);
            rect(j * coarseCellSize, i * coarseCellSize, coarseCellSize, coarseCellSize);
        }
    }
    
    image(fine, 0, 0);
    
    if (false)
    {
        stroke(255, 255, 0);
        noFill();
        triangle(
            controlPoints[0].x, controlPoints[0].y,
            controlPoints[1].x, controlPoints[1].y,
            controlPoints[2].x, controlPoints[2].y);
    }
        
    stroke(255, 0, 0);
    noFill();
    ellipseMode(RADIUS);
    for (PVector p : controlPoints)
        ellipse(p.x, p.y, CONTROL_POINT_RADIUS, CONTROL_POINT_RADIUS);
        
    line(10, 10, 50, 60);
}

void updateCoarse()
{
    for (int i = 0; i < coarseHeight; ++i)
    {
        for (int j = 0; j < coarseWidth; ++j)
        {
            coarse[i][j] = coverage(i, j);
            //coarse[i][j] = sdist(i, j);
        }
    }
}

color valueToColor(float v) { return color(255.0f - v * 255.0f); }

final float aa = sqrt(2) / (4 * coarseCellSize);
final float aamin = 0.5f - 0.5f * aa;
final float aamax = 0.5f + 0.5f * aa;
float threshold(float v)
{
    if (v < aamin)
        return 0;
    if (v > aamax)
        return 1;
    v = (v - aamin) / aa;
    return v * v * (3 - 2 * v);
}

void updateFine()
{
    fine.loadPixels();
    color[] ps = fine.pixels;
    for (int i = 0; i < height; ++i)
    {
        float y = i + 0.5f;
        for (int j = 0; j < width; ++j)
        {
            float x = j + 0.5f;
            float v = getFieldValue(x, y);
            v = threshold(v);
            ps[i * width + j] = valueToColor(v);
        }
    }
    fine.updatePixels();
}

boolean pointInTriangle(PVector p, PVector a, PVector b, PVector c)
{
    float pa = per(span(a, p), span(a, b));
    float pb = per(span(b, p), span(b, c));
    if (pa * pb <= 0.0f)
        return false;
    float pc = per(span(c, p), span(c, a));
    return pa * pc > 0.0f;
}

float coverage(int i, int j)
{
    PVector p0 = new PVector(j, i).mult(coarseCellSize);
    int hits = 0;
    PVector a = controlPoints[0], b = controlPoints[1], c = controlPoints[2];
    for (PVector s : coarseSamples)
    {
        PVector p = PVector.add(p0, s);
        if (pointInTriangle(p, a, b, c))
            ++hits;
    }
    return (float)hits / coarseSamples.length;
}

float distToLine(PVector p, PVector a, PVector b)
{
    return abs(per(span(p, a), span(p, b))) / a.dist(b);
}

float maxDist = sqrt(2) * coarseCellSize;
float distResolution = 64;
float sdist(int i, int j)
{
    PVector p = new PVector(j + 0.5f, i + 0.5f).mult(coarseCellSize);
    PVector a = controlPoints[0], b = controlPoints[1], c = controlPoints[2];
    float d = min(distToLine(p, a, b), distToLine(p, b, c), distToLine(p, c, a));
    if (d > maxDist)
        d = maxDist;
    d = round(d / maxDist * (distResolution - 1)) / (distResolution - 1);
    if (!pointInTriangle(p, a, b, c))
        d = -d;
    return 0.5f + 0.5f * d;
}

PVector grabbedControlPoint;

void mousePressed()
{
    PVector m = new PVector(mouseX, mouseY);
    
    grabbedControlPoint = null;
    for (PVector p : controlPoints)
    {
        if (m.dist(p) <= CONTROL_POINT_RADIUS)
        {
            grabbedControlPoint = p;
            return;
        }
    }
}

void mouseDragged()
{
    if (grabbedControlPoint != null)
    {
        grabbedControlPoint.set(mouseX, mouseY);
        updateCoarse();
    }
}

void mouseReleased()
{
    if (grabbedControlPoint != null)
    {
        updateFine();
        grabbedControlPoint = null;
    }
}