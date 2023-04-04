import { Component, ViewChild } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { Edge, Vertex } from '../utils/interfaces';

interface VData {
  depth: number;
  width: number;
}

@Component({
  selector: 'app-parameter-visualizer',
  templateUrl: './parameter-visualizer.component.html',
  styleUrls: ['./parameter-visualizer.component.css'],
})
export class ParameterVisualizerComponent {
  vertices: Vertex[] = [];

  readonly W: number = 1000;
  readonly R: number = 25;
  readonly W_R: number = this.W / 2 / this.R;

  constructor(private parameterService: ParameterService) {
    console.log(this.getTextWidth('AB', 'bold 1.25em Courier New monospace'));
    this.vertices = this.parameterService.createGraph();
    this.vertices.forEach(
      (v: Vertex) =>
        (v.data = <VData>{
          depth: -1,
          width: this.getTextWidth(v.name, 'bold 1.25em Courier New monospace'),
        })
    );
    this.calcDepths(this.vertices);
    this.vertices.sort((a: Vertex, b: Vertex) => a.data.depth - b.data.depth);
  }

  canvas: HTMLCanvasElement = document.createElement('canvas');

  getTextWidth(text: string, font: string): number {
    // if given, use cached canvas for better performance
    // else, create new canvas
    let context: CanvasRenderingContext2D | null = this.canvas.getContext('2d');
    if (!context) {
      return 0;
    }
    context.font = font;
    var metrics = context.measureText(text);
    return metrics.width;
  }

  calcDepth(v: Vertex): number {
    if (v.data.depth !== -1) {
      return v.data.depth;
    }
    v.data.depth =
      v.in.reduce(
        (max: number, e: Edge) => Math.max(max, this.calcDepth(e.from)),
        0
      ) + 1;
    return v.data.depth;
  }

  calcDepths(vs: Vertex[]) {
    vs.forEach((v: Vertex) => this.calcDepth(v));
  }

  getWidth(v: Vertex, dummy: HTMLElement): number {
    dummy.textContent = v.name;
    let w: number = (dummy as any as SVGTextElement).getBBox().width;
    return Math.min(w, this.R * 2 - 2);
  }

  getCX(v: Vertex, i: number): number {
    return (((i % this.W_R) + 0.5) * this.W) / this.W_R;
  }

  getCY(v: Vertex, i: number): number {
    return ((Math.floor(i / this.W_R) + 0.5) * this.W) / this.W_R;
  }
}
